#include "runtime_gamedata_path.h"

// PATCHED: BitmapFontRenderer.cpp — Format fix, scale, spacing
#include "bitmap_font_renderer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>
#include <stb_image.h>
#include <nlohmann/json.hpp>
#include "math.h"

BitmapFontRenderer::BitmapFontRenderer() {
    LoadFont(gamedata::Texture("font_sfmono_rgba.png"), gamedata::Script("font_sfmono_metadata.json"));
}

using json = nlohmann::json;

static GLuint fontShader = 0;

// === Utility: Load shader source code from file ===
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

// === Utility: Compile a single shader ===
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
    }

    return shader;
}


// ... LoadTextFile and CompileShader unchanged ...

static GLuint LoadFontShader() {
    std::string vs = LoadTextFile(gamedata::Shader("font.vert"));
    std::string fs = LoadTextFile(gamedata::Shader("font.frag"));
    GLuint vert = CompileShader(GL_VERTEX_SHADER, vs, "font.vert");
    GLuint frag = CompileShader(GL_FRAGMENT_SHADER, fs, "font.frag");

    if (vert == 0 || frag == 0) return 0;

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) return 0;

    glDeleteShader(vert);
    glDeleteShader(frag);
    return program;
}

bool BitmapFontRenderer::LoadFont(const std::string& imagePath, const std::string& metaPath) {
    std::ifstream metaFile(metaPath);
    if (!metaFile.is_open()) return false;

    std::stringstream buffer;
    buffer << metaFile.rdbuf();
    json j = json::parse(buffer.str());

    cellWidth = j["cell_width"];
    cellHeight = j["cell_height"];
    atlasWidth = j["image_width"];
    atlasHeight = j["image_height"];

    for (auto& [ch, info] : j["char_data"].items()) {
        if (ch.empty()) continue;
        GlyphInfo glyph = { info["x"], info["y"], info["width"], info["height"] };
        glyphs[ch[0]] = glyph;
    }

    int w, h, channels;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data = stbi_load(imagePath.c_str(), &w, &h, &channels, 0);
    if (!data) return false;

    GLint format = (channels == 4) ? GL_RGBA : GL_RGB;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
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

    if (fontShader == 0) fontShader = LoadFontShader();
    return fontShader != 0;
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
    float scale = (windowH / baseHeight) * 0.85f;

    Mat4 ortho = Mat4::orthographic(0, (float)windowW, (float)windowH, 0, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(fontShader, "uProjection"), 1, GL_FALSE, ortho.toGLMatrix());

    glBindVertexArray(vao);
    int cursorX = x;

    for (char c : text) {
        if (c < 32 || c > 126) continue;
        auto it = glyphs.find(c);
        if (it == glyphs.end()) continue;

        const GlyphInfo& g = it->second;

        float w = g.width * scale;
        float h = g.height * scale;
        float yOffset = (cellHeight - g.height) * scale;

        float xpos = (float)cursorX;
        float ypos = (float)y + yOffset;

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

        cursorX += (int)(w * 1.05f);
    }

    glBindVertexArray(0);
}

void BitmapFontRenderer::RenderConsoleText(const std::string& inputBuffer, const std::vector<std::string>& history, int width, int height, bool blinkVisible) {
    std::string inputText = "> " + inputBuffer;
    if (blinkVisible) inputText += "_";

    float baseHeight = 720.0f;
    float scale = (height / baseHeight) * 0.85f;
    int lineHeight = (int)(cellHeight * scale);
    int maxHeight = static_cast<int>(height * 0.25f);

    int linesToShow = 0;
    int tempY = 40;
    for (int i = (int)history.size() - 1; i >= 0; --i) {
        if (tempY + lineHeight > maxHeight)
            break;
        ++linesToShow;
        tempY += lineHeight;
    }

    int historyY = 40 - lineHeight;
    int startIdx = std::max(0, (int)history.size() - linesToShow);
    for (int i = startIdx; i < (int)history.size(); ++i) {
        RenderText(history[i], 10, historyY, width, height);
        historyY += lineHeight;
    }

    RenderText(inputText, 10, historyY, width, height);
}
