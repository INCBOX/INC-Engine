#include "mathlib/camera.h"
#include <SDL2/SDL.h>
#include <cmath>

Camera::Camera()
{
    position = Vec3(0.0f, 0.0f, 3.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    cameraSpeed = 5.0f;
    mouseSensitivity = 0.1f;
}

void Camera::Update(float deltaTime, const Uint8* keystate, int mouseDX, int mouseDY)
{
    yaw += mouseDX * mouseSensitivity;
    pitch -= mouseDY * mouseSensitivity; // Subtract for natural up/down

    // Clamp pitch to prevent flipping
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Convert to forward vector
    float radYaw = yaw * (3.14159265f / 180.0f);
    float radPitch = pitch * (3.14159265f / 180.0f);
    forward = Vec3(
        cos(radYaw) * cos(radPitch),
        sin(radPitch),
        sin(radYaw) * cos(radPitch)
    ).Normalize();

    // Flat movement plane
    Vec3 flatForward = Vec3(forward.x, 0.0f, forward.z).Normalize();
    Vec3 right = flatForward.Cross(Vec3(0.0f, 1.0f, 0.0f)).Normalize();

    if (keystate[SDL_SCANCODE_W]) position += flatForward * cameraSpeed * deltaTime;
    if (keystate[SDL_SCANCODE_S]) position -= flatForward * cameraSpeed * deltaTime;
    if (keystate[SDL_SCANCODE_A]) position -= right * cameraSpeed * deltaTime;
    if (keystate[SDL_SCANCODE_D]) position += right * cameraSpeed * deltaTime;
}

Mat4 Camera::GetViewMatrix() const
{
    return Matrix::LookAt(position, position + forward, Vec3(0.0f, 1.0f, 0.0f));
}
