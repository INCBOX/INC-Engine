#pragma once
#include <SDL2/SDL.h>

class Input {
public:
    void Init();
    void Shutdown();

    void Update();

    // Getters for key state and mouse delta
    const Uint8* GetKeyState() const { return m_Keystate; }
    int GetMouseDeltaX() const { return m_MouseDeltaX; }
    int GetMouseDeltaY() const { return m_MouseDeltaY; }

private:
    const Uint8* m_Keystate = nullptr;
    int m_MouseDeltaX = 0;
    int m_MouseDeltaY = 0;

    int m_LastMouseX = 0;
    int m_LastMouseY = 0;
};
