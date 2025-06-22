#pragma once

#include "mathlib/vector3_d.h"
#include "mathlib/matrix4x4_f.h" // We still render using float
#include <SDL.h>

class Camera_d {
public:
    Camera_d();

    void Update(float deltaTime, const Uint8* keystate, int mouseDX, int mouseDY);

    // World-space position
    void SetWorldPosition(const Vector3_d& pos);
    Vector3_d GetWorldPosition() const;

    // Relative render-space data
    Vector3_f GetRenderPosition() const;

    Matrix4x4_f GetViewMatrix() const;
    Matrix4x4_f GetProjectionMatrix(float aspectRatio) const;

    void AddYaw(float degrees);
    void AddPitch(float degrees);
    void ClampPitch(float min, float max);

    Vector3_f GetForwardVector() const;
    Vector3_f GetRightVector() const;

private:
    Vector3_d m_worldPosition; // True position in simulation space
    Vector3_f m_forward;       // View direction (unit vector)
    float m_yaw;
    float m_pitch;
    float m_speed;
    float m_mouseSensitivity;

    // Cached origin offset for render-space
    Vector3_d m_simulationOrigin;
    float m_fovDegrees = 120.0f;
    float m_zNear = 0.1f;
    float m_zFar = 100000.0f;
};