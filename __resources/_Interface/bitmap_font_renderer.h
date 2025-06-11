#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <glad/glad.h> // Replaces <GL/gl.h>, works with your engine

struct GlyphInfo {
    int x, y, width, height;
};

class BitmapFontRenderer {
public:
    BitmapFontRenderer();
    void RenderConsoleText(const std::string& inputBuffer, const std::vector<std::string>& history, int width, int height, bool blinkVisible);
    void SetColor(float r, float g, float b);
    bool LoadFont(const std::string& imagePath, const std::string& metaPath);
    void RenderText(const std::string& text, int x, int y, int windowW, int windowH);

private:
    std::unordered_map<char, GlyphInfo> glyphs;
    int atlasWidth = 0;
    int atlasHeight = 0;
    int cellWidth = 0;
    int cellHeight = 0;
    GLuint textureID = 0;

    // Modern OpenGL data
    GLuint vao = 0;
    GLuint vbo = 0;
};