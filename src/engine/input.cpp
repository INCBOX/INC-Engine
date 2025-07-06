#include "input.h"
#include <SDL2/SDL.h>

void Input::Init() {
    SDL_PumpEvents();
    m_Keystate = SDL_GetKeyboardState(nullptr);

    SDL_SetRelativeMouseMode(SDL_TRUE); // ⬅️ Locks cursor & gives relative movement!
}

void Input::Shutdown() {
    // SDL_SetRelativeMouseMode(SDL_FALSE); // DISABLED: don't turn off relative mode during gameplay
}

void Input::Update() {
    SDL_PumpEvents(); // MOUSE MOVEMENT STUFF
    m_Keystate = SDL_GetKeyboardState(nullptr);

    // This gives smooth relative deltas regardless of screen edge
    m_MouseDeltaX = 0;
    m_MouseDeltaY = 0;
    SDL_GetRelativeMouseState(&m_MouseDeltaX, &m_MouseDeltaY);
}

void Input::ProcessSDLEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            // Update internal key states or mark key pressed/released as needed
            break;
        case SDL_MOUSEMOTION:
            // Maybe update mouse delta here, or accumulate it
            break;
        // handle other events if needed
    }
}