#include <glad/glad.h>   // MUST come first before using OpenGL types
#include <stb_image.h>
#include <nlohmann/json.hpp>

#include "runtime_gamedata_path.h"
#include "engine_globals.h"
#include "mathlib/Mat4.h"
#include "bitmap_font_renderer.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

using namespace mathlib;

BitmapFontRenderer::BitmapFontRenderer() {
    LoadFont(gamedata::Texture("font_sfmono_rgba.png"), gamedata::Script("font_sfmono_metadata.json"));
}

static GLuint fontShader = 0;

// === Embedded shader ===
static GLuint LoadFontShader() {
    const char* vs = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoord;
        uniform mat4 uProjection;
        out vec2 TexCoord;
        void main() {
            gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const char* fs = R"(
        #version 330 core
        in vec2 TexCoord;
        uniform sampler2D uFontAtlas;
        uniform vec3 uTextColor;
        out vec4 FragColor;
        void main() {
            vec4 sampled = texture(uFontAtlas, TexCoord);
            FragColor = vec4(uTextColor, sampled.r); // Grayscale font
        }
    )";

    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vs, nullptr);
    glCompileShader(vert);
    GLint success;
    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(vert, 512, nullptr, log);
        std::cerr << "[Font] Vertex Shader compile error: " << log << std::endl;
    }

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fs, nullptr);
    glCompileShader(frag);
    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(frag, 512, nullptr, log);
        std::cerr << "[Font] Fragment Shader compile error: " << log << std::endl;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    glDeleteShader(vert);
    glDeleteShader(frag);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "[Font] Shader link error: " << log << std::endl;
    }

    return program;
}

bool BitmapFontRenderer::LoadFont(const std::string& imagePath, const std::string& metaPath) {
    std::ifstream metaFile(metaPath);
    if (!metaFile.is_open()) return false;

    nlohmann::json j;
    metaFile >> j;

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

    Mat4 ortho = Mat4::Orthographic(0, (float)windowW, (float)windowH, 0, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(fontShader, "uProjection"), 1, GL_FALSE, ortho.Data());

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
