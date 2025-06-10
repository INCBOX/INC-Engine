#pragma once

#include "Math.h"
#include <SDL2/SDL.h>

class FPSCamera {
public:
    Vec3 Position;
    float Yaw, Pitch;

    FPSCamera();
    FPSCamera(const Vec3& startPos);

    void Update(float deltaTime); // Now used for smoothing only
    void ProcessInput(const Uint8* keys, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset);
    Mat4 GetViewMatrix() const;

private:
    Vec3 Front, Up, Right;
    void updateVectors();

    float SmoothedX = 0.0f;
    float SmoothedY = 0.0f;

    const float MouseSensitivity = 0.1f;
    const float MouseSmoothing = 0.5f;
};
