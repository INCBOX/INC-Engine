
#pragma once

#include <string>
#include <vector>
#include "HUD.h"
#include "../Fonts/BitmapFontRenderer.h"

class Console {
public:
    Console();
    Console(HUD* hud);

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

    HUD* hudRef;
    BitmapFontRenderer font;

    // === Blinking cursor support ===
    float blinkTimer = 0.0f;
    bool blinkVisible = true;
};
