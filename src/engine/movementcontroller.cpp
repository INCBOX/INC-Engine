#include "movementcontroller.h"
#include <SDL.h>
#include <cmath>

MovementController::MovementController() {
    m_Speed = 5.0f;
}

void MovementController::Update(float dt, const Input& input, Vector& position, const Vector& forward, const Vector& right) {
    Vector flatForward = Vector(forward.x, 0, forward.z).Normalize();
    Vector movement;

    const Uint8* keys = input.GetKeyState();

    if (keys[SDL_SCANCODE_W]) movement += flatForward;
    if (keys[SDL_SCANCODE_S]) movement -= flatForward;
    if (keys[SDL_SCANCODE_A]) movement -= right;
    if (keys[SDL_SCANCODE_D]) movement += right;

    if (movement.Length() > 0.01f)
        movement = movement.Normalize() * m_Speed * dt;

    position += movement;
}
