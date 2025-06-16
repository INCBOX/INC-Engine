#pragma once
#include "mathlib/camera.h"

class CameraController {
public:
    CameraController();

    // Called each frame with input state & mouse deltas
    void Update(float deltaTime, const Uint8* keystate, int mouseDeltaX, int mouseDeltaY);

    // Get current camera
    const Camera& GetCamera() const;

private:
    Camera m_Camera;

    float m_MouseSensitivity = 0.1f;
    float m_MoveSpeed = 5.0f;
};