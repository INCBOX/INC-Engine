#pragma once
#include <SDL.h>
#include "mathlib/vector.h"
#include "mathlib/matrix.h"

class Camera
{
public:
    Camera();

    void Update(float deltaTime, const Uint8* keystate, int mouseDeltaX, int mouseDeltaY);

    Matrix GetViewMatrix() const;

    Vector GetPosition() const { return position; }
    Vector GetForward() const { return forward; }
    Vector GetUp() const { return up; }

private:
    void UpdateForwardVector();

    Vector position;
    Vector forward;
    Vector up;

    float yaw;
    float pitch;

    static constexpr float cameraSpeed = 5.0f;
    static constexpr float mouseSensitivity = 0.002f;
};
