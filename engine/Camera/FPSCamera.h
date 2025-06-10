#pragma once
#include "math.h"
#include <SDL2/SDL.h>

class FPSCamera {
public:
    Vec3 Position;
    float Pitch = 0.0f;
    float Yaw = -90.0f;
    float MouseSensitivity = 0.12f;
    float MouseSmoothing = 0.4f;

    FPSCamera();
    FPSCamera(const Vec3& startPos);

    void Update(float deltaTime);
    void ProcessInput(const Uint8* keys, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset);
    Mat4 GetViewMatrix() const;
    Vec3 GetFront() const { return Front; }  // <-- added getter

private:
    Vec3 Front, Up, Right;
    float SmoothedX = 0.0f;
    float SmoothedY = 0.0f;

    void updateVectors();
};
