#pragma once

#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include "Fonts/BitmapFontRenderer.h"

class Console {
public:
    bool InitShader(); // ✅ Load console.vert and console.frag into consoleShader
    bool InitFont();
    Console();

    void Toggle();
    bool IsActive() const;

    void ProcessEvent(const SDL_Event& event);
    void ExecuteCommand(const std::string& command);
    void Update(float deltaTime);
    void Render(int width, int height);

private:
    bool active;
    std::string inputBuffer;
    std::vector<std::string> commandHistory;
    int historyIndex;

    BitmapFontRenderer font;
};
