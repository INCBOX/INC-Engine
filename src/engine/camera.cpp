#include "mathlib/camera.h"
#include <SDL.h>
#include <cmath>

Camera::Camera()
    : position(0.0f, 0.0f, 5.0f),
      up(0.0f, 1.0f, 0.0f),
      yaw(3.14159f),  // pointing along -Z
      pitch(0.0f)
{
    UpdateForwardVector();
}

void Camera::UpdateForwardVector()
{
    Vector dir;
    dir.x = cosf(pitch) * sinf(yaw);
    dir.y = sinf(pitch);
    dir.z = cosf(pitch) * cosf(yaw);

    forward = dir.Normalize();
}

void Camera::Update(float deltaTime, const Uint8* keystate, int mouseDeltaX, int mouseDeltaY)
{
    // Update rotation based on mouse input
    yaw += mouseDeltaX * mouseSensitivity;
    pitch -= mouseDeltaY * mouseSensitivity;

    const float pitchLimit = 1.5f; // ~85 degrees
    if (pitch > pitchLimit) pitch = pitchLimit;
    if (pitch < -pitchLimit) pitch = -pitchLimit;

    UpdateForwardVector();

    // Calculate right vector
    Vector right = forward.Cross(up).Normalize();

    // Update position based on keyboard input
    if (keystate[SDL_SCANCODE_W])
        position = position + forward * cameraSpeed * deltaTime;
    if (keystate[SDL_SCANCODE_S])
        position = position - forward * cameraSpeed * deltaTime;
    if (keystate[SDL_SCANCODE_A])
        position = position - right * cameraSpeed * deltaTime;
    if (keystate[SDL_SCANCODE_D])
        position = position + right * cameraSpeed * deltaTime;
}

Matrix Camera::GetViewMatrix() const
{
    Vector right = forward.Cross(up).Normalize();
    Vector trueUp = right.Cross(forward).Normalize();

    Matrix view = Matrix::Identity();

    view.m[0][0] = right.x;    view.m[1][0] = right.y;    view.m[2][0] = right.z;
    view.m[0][1] = trueUp.x;   view.m[1][1] = trueUp.y;   view.m[2][1] = trueUp.z;
    view.m[0][2] = -forward.x; view.m[1][2] = -forward.y; view.m[2][2] = -forward.z;

    view.m[3][0] = -position.Dot(right);
    view.m[3][1] = -position.Dot(trueUp);
    view.m[3][2] = position.Dot(forward);

    return view;
}