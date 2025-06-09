#include "BitmapFontRenderer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>
#include <stb_image.h>
#include <nlohmann/json.hpp>
#include "../Math.h"
using json = nlohmann::json;

static GLuint fontShader = 0;

static std::string LoadTextFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[Font] Failed to load shader: " << path << std::endl;
        return "";
    }
    std::stringstream s;
    s << file.rdbuf();
    return s.str();
}

static GLuint CompileShader(GLenum type, const std::string& source, const std::string& name) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "[Font] Shader compile error (" << name << "): " << log << std::endl;
        return 0;
    } else {
        std::cout << "[Font] Shader compiled successfully: " << name << std::endl;
    }

    return shader;
}

static GLuint LoadFontShader() {
    std::string vs = LoadTextFile("bin/resources/shaders/font.vert");
    std::string fs = LoadTextFile("bin/resources/shaders/font.frag");

    GLuint vert = CompileShader(GL_VERTEX_SHADER, vs, "font.vert");
    GLuint frag = CompileShader(GL_FRAGMENT_SHADER, fs, "font.frag");

    if (vert == 0 || frag == 0) {
        std::cerr << "[Font] Shader compile failure. Aborting shader program creation." << std::endl;
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "[Font] Shader link error: " << log << std::endl;
        return 0;
    } else {
        std::cout << "[Font] Shader program linked successfully.\n";
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
    return program;
}

bool BitmapFontRenderer::LoadFont(const std::string& imagePath, const std::string& metaPath) {
    std::ifstream metaFile(metaPath);
    if (!metaFile.is_open()) {
        std::cerr << "Failed to load font metadata: " << metaPath << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << metaFile.rdbuf();
    json j = json::parse(buffer.str());

    if (!j["char_data"].is_structured()) {
        std::cerr << "'char_data' is not structured properly in font metadata JSON\n";
        return false;
    }

    cellWidth = j["cell_width"];
    cellHeight = j["cell_height"];
    atlasWidth = j["image_width"];
    atlasHeight = j["image_height"];

    for (auto& [ch, info] : j["char_data"].items()) {
        if (ch.empty()) continue;
        GlyphInfo glyph;
        glyph.x = info["x"];
        glyph.y = info["y"];
        glyph.width = info["width"];
        glyph.height = info["height"];
        glyphs[ch[0]] = glyph;
    }

    int w, h, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(imagePath.c_str(), &w, &h, &channels, 0);
    if (!data) {
        std::cerr << "[Font] Failed to load font atlas: " << imagePath << std::endl;
        return false;
    }

    std::cout << "[Font] Font atlas loaded: " << imagePath << " (" << w << "x" << h << "), channels: " << channels << std::endl;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(data);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    if (fontShader == 0) {
        fontShader = LoadFontShader();
        if (fontShader == 0) {
            std::cerr << "[Font] ERROR: fontShader is 0 (shader load failed)\n";
            return false;
        }
    }

    return true;
}

void BitmapFontRenderer::RenderText(const std::string& text, int x, int y, int windowW, int windowH) {
    if (textureID == 0 || vao == 0 || vbo == 0 || fontShader == 0) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(fontShader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(fontShader, "uFontAtlas"), 0);
    glUniform3f(glGetUniformLocation(fontShader, "uTextColor"), 1.0f, 1.0f, 1.0f);

    float baseHeight = 720.0f;
    float scale = windowH / baseHeight;

    Mat4 ortho = Mat4::orthographic(0, (float)windowW, (float)windowH, 0, -1.0f, 1.0f);
    GLint projLoc = glGetUniformLocation(fontShader, "uProjection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, ortho.toGLMatrix());

    glBindVertexArray(vao);

    int cursorX = x;

    for (char c : text) {
        // Skip characters outside printable ASCII range
        if (c < 32 || c > 126) continue;
        auto it = glyphs.find(c);
        if (it == glyphs.end()) continue;

        const GlyphInfo& g = it->second;

        float xpos = (float)cursorX;
        float ypos = (float)y;
        float w = g.width * scale;
        float h = g.height * scale;

        float u0 = (float)g.x / atlasWidth;
        float v0 = (float)g.y / atlasHeight;
        float u1 = (float)(g.x + g.width) / atlasWidth;
        float v1 = (float)(g.y + g.height) / atlasHeight;

        float verts[6][4] = {
            { xpos,     ypos,     u0, v0 },
            { xpos + w, ypos,     u1, v0 },
            { xpos + w, ypos + h, u1, v1 },
            { xpos,     ypos,     u0, v0 },
            { xpos + w, ypos + h, u1, v1 },
            { xpos,     ypos + h, u0, v1 },
        };

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        cursorX += (int)(g.width * scale);
    }

    glBindVertexArray(0);
}
