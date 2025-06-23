#include "mathlib/camera_d.h"
#include <cmath>

static constexpr double DEG2RAD = 3.14159265358979323846 / 180.0;

Camera_d::Camera_d()
{
    position = Vector3_d(0.0, 0.0, 3.0);
    yaw = -90.0;
    pitch = 0.0;
    cameraSpeed = 5.0;
    mouseSensitivity = 0.1;
    forward = Vector3_d(0.0, 0.0, -1.0);
}

void Camera_d::Update(double deltaTime, const Uint8* keystate, int mouseDX, int mouseDY)
{
    yaw += mouseDX * mouseSensitivity;
    pitch -= mouseDY * mouseSensitivity;

    ClampPitch(-89.0, 89.0);

    double radYaw = yaw * DEG2RAD;
    double radPitch = pitch * DEG2RAD;
    forward = Vector3_d(
        cos(radYaw) * cos(radPitch),
        sin(radPitch),
        sin(radYaw) * cos(radPitch)
    ).Normalize();

    Vector3_d flatForward = Vector3_d(forward.x, 0.0, forward.z).Normalize();

    Vector3_d right = flatForward.Cross(Vector3_d(0.0, 1.0, 0.0)).Normalize();

    if (keystate[SDL_SCANCODE_W]) position += flatForward * cameraSpeed * deltaTime;
    if (keystate[SDL_SCANCODE_S]) position -= flatForward * cameraSpeed * deltaTime;
    if (keystate[SDL_SCANCODE_A]) position -= right * cameraSpeed * deltaTime;
    if (keystate[SDL_SCANCODE_D]) position += right * cameraSpeed * deltaTime;
}

Matrix4x4_d Camera_d::GetViewMatrix() const
{
    return Matrix4x4_d::LookAt(position, position + forward, Vector3_d(0.0, 1.0, 0.0));
}

void Camera_d::SetPosition(const Vector3_d& pos)
{
    position = pos;
}

Vector3_d Camera_d::GetPosition() const
{
    return position;
}

void Camera_d::AddYaw(double degrees)
{
    yaw += degrees;
}

void Camera_d::AddPitch(double degrees)
{
    pitch += degrees;
}

void Camera_d::ClampPitch(double minDegrees, double maxDegrees)
{
    if (pitch < minDegrees) pitch = minDegrees;
    if (pitch > maxDegrees) pitch = maxDegrees;
}

Vector3_d Camera_d::GetForwardVector() const
{
    return forward;
}

Vector3_d Camera_d::GetRightVector() const
{
    return forward.Cross(Vector3_d(0.0, 1.0, 0.0)).Normalize();
}

void Camera_d::UpdateOrientation()
{
    double radYaw = yaw * DEG2RAD;
    double radPitch = pitch * DEG2RAD;

    forward = Vector3_d(
        cos(radYaw) * cos(radPitch),
        sin(radPitch),
        sin(radYaw) * cos(radPitch)
    ).Normalize();
}

Matrix4x4_d Camera_d::GetProjectionMatrix(double aspect) const
{
    double fovY = 2.0 * atan(tan(m_fovDegrees * 0.5 * DEG2RAD) / aspect) * (180.0 / 3.14159265358979323846);
    return Matrix4x4_d::Perspective(fovY, aspect, m_zNear, m_zFar);
}

void Camera_d::UpdateRotationOnly(double deltaTime, int mouseDX, int mouseDY)
{
    yaw += mouseDX * mouseSensitivity;
    pitch -= mouseDY * mouseSensitivity;
    ClampPitch(-89.0, 89.0);
    UpdateOrientation(); // recompute forward vector
}