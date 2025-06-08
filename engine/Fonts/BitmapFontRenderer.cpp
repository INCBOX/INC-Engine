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

    json charData = j["char_data"];
    for (int i = 0; i < charData.size(); ++i) {
        // In old JSON versions you can only iterate by keys manually
        // So we re-parse the original stream with an object loop
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
        break; // Exit after 1 loop (since we looped manually)
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

    return true;
}

void BitmapFontRenderer::RenderText(const std::string& text, int x, int y, int windowW, int windowH) {
    // TODO: Use modern OpenGL to render text.
}