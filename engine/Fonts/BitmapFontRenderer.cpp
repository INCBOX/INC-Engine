#include "BitmapFontRenderer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>
#include <stb_image.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

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

    for (auto it = j["char_data"].items().begin(); it != j["char_data"].items().end(); ++it) {
        std::string ch = it.key();
        json info = it.value();

        GlyphInfo glyph;
        glyph.x = info["x"];
        glyph.y = info["y"];
        glyph.width = info["width"];
        glyph.height = info["height"];
        glyphs[ch[0]] = glyph;
    }

    int w, h, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(imagePath.c_str(), &w, &h, &channels, 1);
    if (!data) {
        std::cerr << "Failed to load font atlas: " << imagePath << std::endl;
        return false;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(data);

    // Set up VAO/VBO for dynamic glyph quads
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW); // 6 vertices * 4 floats (pos + uv)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

void BitmapFontRenderer::RenderText(const std::string& text, int x, int y, int windowW, int windowH) {
    if (textureID == 0 || vao == 0 || vbo == 0) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(vao);

    int cursorX = x;

    for (char c : text) {
        auto it = glyphs.find(c);
        if (it == glyphs.end()) continue;

        const GlyphInfo& g = it->second;

        float xpos = static_cast<float>(cursorX);
        float ypos = static_cast<float>(y);
        float w = static_cast<float>(g.width);
        float h = static_cast<float>(g.height);

        float u0 = (float)g.x / atlasWidth;
        float v0 = (float)g.y / atlasHeight;
        float u1 = (float)(g.x + g.width) / atlasWidth;
        float v1 = (float)(g.y + g.height) / atlasHeight;

        float screenW = (float)windowW;
        float screenH = (float)windowH;

        float ndcX = (xpos / screenW) * 2.0f - 1.0f;
        float ndcY = 1.0f - (ypos / screenH) * 2.0f;
        float ndcW = (w / screenW) * 2.0f;
        float ndcH = (h / screenH) * 2.0f;

        float verts[6][4] = {
            { ndcX,         ndcY,         u0, v0 },
            { ndcX + ndcW,  ndcY,         u1, v0 },
            { ndcX + ndcW,  ndcY - ndcH,  u1, v1 },

            { ndcX,         ndcY,         u0, v0 },
            { ndcX + ndcW,  ndcY - ndcH,  u1, v1 },
            { ndcX,         ndcY - ndcH,  u0, v1 },
        };

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        cursorX += g.width;
    }

    glBindVertexArray(0);
}