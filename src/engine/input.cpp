#include "input.h"
#include <SDL.h>

void Input::Init() {
    SDL_PumpEvents();
    m_Keystate = SDL_GetKeyboardState(nullptr);

    SDL_SetRelativeMouseMode(SDL_TRUE); // ⬅️ Locks cursor & gives relative movement!
}

void Input::Shutdown() {
    SDL_SetRelativeMouseMode(SDL_FALSE);
}

void Input::Update() {
    SDL_PumpEvents();
    m_Keystate = SDL_GetKeyboardState(nullptr);

    // This gives smooth relative deltas regardless of screen edge
    m_MouseDeltaX = 0;
    m_MouseDeltaY = 0;
    SDL_GetRelativeMouseState(&m_MouseDeltaX, &m_MouseDeltaY);
}
