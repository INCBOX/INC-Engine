#pragma once

#include "../Math/Math.h"
#include <SDL2/SDL.h>

class FPSCamera {
public:
    Vec3 Position;
    Vec3 Velocity;
    Vec3 Front;
    Vec3 Up;
    Vec3 Right;

    float Yaw;
    float Pitch;
    bool IsGrounded;

    FPSCamera();

    void Update(float deltaTime);
    void ProcessInput(const Uint8* keyState, float deltaTime);
    Mat4 GetViewMatrix() const;
    void ProcessMouseMovement(float xoffset, float yoffset);

private:
    void updateVectors();

    // Mouse smoothing state
    float SmoothedX = 0.0f;
    float SmoothedY = 0.0f;

    // Settings
    float MouseSensitivity = 0.08f;
    float MouseSmoothing = 0.4f; // Range: 0 (raw) to ~0.9 (smooth)
};
