#pragma once

#include <string>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <glad/glad.h> // ✅ Replaces <GL/gl.h>, works with your engine

struct GlyphInfo {
    int x, y, width, height;
};

class BitmapFontRenderer {
public:
    bool LoadFont(const std::string& imagePath, const std::string& metaPath);
    void RenderText(const std::string& text, int x, int y, int windowW, int windowH);

private:
    std::unordered_map<char, GlyphInfo> glyphs;
    int atlasWidth = 0;
    int atlasHeight = 0;
    int cellWidth = 0;
    int cellHeight = 0;
    GLuint textureID = 0;
};
