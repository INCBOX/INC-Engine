#pragma once

#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include "Fonts/BitmapFontRenderer.h"

class HUD; // Forward declaration

class Console {
public:
    Console();
    Console(HUD* hud); // ✅ New constructor

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
    HUD* hudRef = nullptr; // ✅ Store pointer to HUD
};