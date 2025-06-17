#pragma once
#include "mathlib/vector.h"
#include "mathlib/matrix.h"
#include <SDL.h>
class Camera
{
public:
    Camera();

    void Update(float deltaTime, const Uint8* keystate, int mouseDX, int mouseDY);
    Mat4 GetViewMatrix() const;

private:
    Vec3 position;
    Vec3 forward;
    float yaw, pitch;
    float cameraSpeed;
    float mouseSensitivity;
};
