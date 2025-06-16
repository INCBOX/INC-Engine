#include "camera_controller.h"
#include <SDL.h>

CameraController::CameraController() {
    // Initialize camera to starting position & orientation
    m_Camera.SetPosition({0.0f, 0.0f, 5.0f});
    m_Camera.SetRotation({0, 0, 0});
}

void CameraController::Update(float deltaTime, const Uint8* keystate, int mouseDeltaX, int mouseDeltaY) {
    // Mouse look
    float yaw = mouseDeltaX * m_MouseSensitivity;
    float pitch = mouseDeltaY * m_MouseSensitivity;

    m_Camera.Rotate(pitch, yaw, 0.0f);

    // Keyboard movement (WASD)
    Vector forward = m_Camera.GetForward();
    Vector right = m_Camera.GetRight();
    Vector up = {0, 1, 0};

    Vector movement = {0,0,0};

    if (keystate[SDL_SCANCODE_W]) movement += forward;
    if (keystate[SDL_SCANCODE_S]) movement -= forward;
    if (keystate[SDL_SCANCODE_D]) movement += right;
    if (keystate[SDL_SCANCODE_A]) movement -= right;
    if (keystate[SDL_SCANCODE_SPACE]) movement += up;
    if (keystate[SDL_SCANCODE_LCTRL]) movement -= up;

    if (movement.LengthSquared() > 0) {
        movement = movement.Normalized() * m_MoveSpeed * deltaTime;
        m_Camera.Translate(movement);
    }
}

const Camera& CameraController::GetCamera() const {
    return m_Camera;
}