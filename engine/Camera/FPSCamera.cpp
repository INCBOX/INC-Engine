#include "FPSCamera.h"
#include <cmath>
#include <iostream>

float gCameraSpeed = 100.0f; // Default speed, adjustable from console

FPSCamera::FPSCamera()
    : Position{0.0f, 0.0f, 10.0f}, Yaw(-90.0f), Pitch(0.0f)
{
    updateVectors();
}

FPSCamera::FPSCamera(const Vec3& startPos)
    : Position(startPos), Yaw(-90.0f), Pitch(0.0f)
{
    updateVectors();
}

void FPSCamera::Update(float /*deltaTime*/) {
    // No gravity, no physics needed
}

void FPSCamera::ProcessInput(const Uint8* keys, float deltaTime)
{
    Vec3 move(0, 0, 0);
    float velocity = gCameraSpeed * deltaTime;

    if (keys[SDL_SCANCODE_LSHIFT]) {
        velocity *= 3.0f; // Boost
    }

    if (keys[SDL_SCANCODE_W]) move += Front;
    if (keys[SDL_SCANCODE_S]) move -= Front;
    if (keys[SDL_SCANCODE_A]) move -= Right;
    if (keys[SDL_SCANCODE_D]) move += Right;
    if (keys[SDL_SCANCODE_Q]) move -= Up;
    if (keys[SDL_SCANCODE_E]) move += Up;

    if (move.length() > 0.0f)
        move = move.normalized();

    Position += move * velocity;
}

void FPSCamera::ProcessMouseMovement(float xoffset, float yoffset)
{
    yoffset = -yoffset;

    SmoothedX = SmoothedX * MouseSmoothing + xoffset * (1.0f - MouseSmoothing);
    SmoothedY = SmoothedY * MouseSmoothing + yoffset * (1.0f - MouseSmoothing);

    Yaw   += SmoothedX * MouseSensitivity;
    Pitch += SmoothedY * MouseSensitivity;

    if (Pitch > 89.0f)  Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    updateVectors();
}

Mat4 FPSCamera::GetViewMatrix() const {
    return Mat4::lookAt(Position, Position + Front, Up);
}

void FPSCamera::updateVectors()
{
    float yawRad = radians(Yaw);
    float pitchRad = radians(Pitch);

    Vec3 front;
    front.x = cosf(pitchRad) * cosf(yawRad);
    front.y = sinf(pitchRad);
    front.z = cosf(pitchRad) * sinf(yawRad);

    Front = front.normalized();
    Right = Front.cross(Vec3{0.0f, 1.0f, 0.0f}).normalized();
    Up    = Right.cross(Front).normalized();
}
