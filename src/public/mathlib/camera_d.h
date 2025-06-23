#pragma once
#include "mathlib/vector3_d.h"
#include "mathlib/matrix4x4_d.h"
#include <SDL.h>

class Camera_d
{
public:
    Camera_d();

    // Update camera orientation and position based on input
    void Update(double deltaTime, const Uint8* keystate, int mouseDX, int mouseDY);

    // Get the view matrix for rendering
    Matrix4x4_d GetViewMatrix() const;

    // PLAYER //////////////////////////////////////
    void SetPosition(const Vector3_d& pos);
    Vector3_d GetPosition() const;
    void AddYaw(double degrees);
    void AddPitch(double degrees);
    void ClampPitch(double minDegrees, double maxDegrees);
    Vector3_d GetForwardVector() const;
    Vector3_d GetRightVector() const;

    // PLAYER CONTROLS CAMERA
    void UpdateOrientation();

    // CAMERA MVP SETUP
    Matrix4x4_d GetProjectionMatrix(double aspectRatio) const;
	
	void UpdateRotationOnly(double dt, int mouseDX, int mouseDY);

private:
    Vector3_d position;
    Vector3_d forward;
    double yaw;
    double pitch;
    double cameraSpeed;
    double mouseSensitivity;

    double m_fovDegrees = 120.0;
    double m_zNear = 0.1;
    double m_zFar = 1000000.0; // far plane extended for space-scale scenes
};