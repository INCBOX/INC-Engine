#include "mathlib/vector3_f.h"
#include "mathlib/matrix4x4_f.h"
#include "mathlib/camera_f.h"
#include <SDL2/SDL.h>
#include <cmath>

// Converts degrees to radians for trigonometric functions
static constexpr float DEG2RAD = 3.14159265f / 180.0f;

Camera_f::Camera_f()
{
    position = Vector3_f(0.0f, 0.0f, 3.0f);
    yaw = -90.0f;         // Facing along negative Z by default
    pitch = 0.0f;
    cameraSpeed = 5.0f;
    mouseSensitivity = 0.1f;
    forward = Vector3_f(0.0f, 0.0f, -1.0f);
}

// Update camera orientation and position based on input
void Camera_f::Update(float deltaTime, const Uint8* keystate, int mouseDX, int mouseDY)
{
    // Update yaw and pitch based on mouse movement
    yaw += mouseDX * mouseSensitivity;
    pitch -= mouseDY * mouseSensitivity; // Subtract for natural up/down movement

    // Clamp pitch to prevent the camera from flipping over
    ClampPitch(-89.0f, 89.0f);

    // Calculate the new forward vector from yaw and pitch
    float radYaw = yaw * DEG2RAD;
    float radPitch = pitch * DEG2RAD;
    forward = Vector3_f(
        cosf(radYaw) * cosf(radPitch),
        sinf(radPitch),
        sinf(radYaw) * cosf(radPitch)
    ).Normalize();

    // Calculate flat forward vector for movement (ignore vertical component)
    Vector3_f flatForward = Vector3_f(forward.x, 0.0f, forward.z).Normalize();

    // Calculate right vector as perpendicular to flat forward
    Vector3_f right = flatForward.Cross(Vector3_f(0.0f, 1.0f, 0.0f)).Normalize();

    // Movement input - WASD keys
    if (keystate[SDL_SCANCODE_W]) position += flatForward * cameraSpeed * deltaTime;
    if (keystate[SDL_SCANCODE_S]) position -= flatForward * cameraSpeed * deltaTime;
    if (keystate[SDL_SCANCODE_A]) position -= right * cameraSpeed * deltaTime;
    if (keystate[SDL_SCANCODE_D]) position += right * cameraSpeed * deltaTime;
}

// Get the view matrix used for rendering the scene from the camera's perspective
Matrix4x4_f Camera_f::GetViewMatrix() const
{
    // Assuming your mathlib Matrix class has a LookAt static method
    return Matrix4x4_f::LookAt(position, position + forward, Vector3_f(0.0f, 1.0f, 0.0f));
}

// Set the camera's absolute position
void Camera_f::SetPosition(const Vector3_f& pos)
{
    position = pos;
}

// Get the camera's current position
Vector3_f Camera_f::GetPosition() const
{
    return position;
}

// Add to yaw angle (rotation around Y axis)
void Camera_f::AddYaw(float degrees)
{
    yaw += degrees;
}

// Add to pitch angle (rotation around X axis)
void Camera_f::AddPitch(float degrees)
{
    pitch += degrees;
}

// Clamp pitch angle to prevent camera from flipping
void Camera_f::ClampPitch(float minDegrees, float maxDegrees)
{
    if (pitch < minDegrees) pitch = minDegrees;
    if (pitch > maxDegrees) pitch = maxDegrees;
}

// Get the forward direction vector
Vector3_f Camera_f::GetForwardVector() const
{
    return forward;
}

// Get the right direction vector (perpendicular to forward and up)
Vector3_f Camera_f::GetRightVector() const
{
    // Right vector = forward cross up
    Vector3_f up(0.0f, 1.0f, 0.0f);
    return forward.Cross(up).Normalize();
}

// PLAYER USES CAMERA
void Camera_f::UpdateOrientation()
{
    float radYaw = yaw * DEG2RAD;
    float radPitch = pitch * DEG2RAD;

    forward = Vector3_f(
        cosf(radYaw) * cosf(radPitch),
        sinf(radPitch),
        sinf(radYaw) * cosf(radPitch)
    ).Normalize();
}

// MVP Setup FOR camera
Matrix4x4_f Camera_f::GetProjectionMatrix(float aspect) const {
	
    // TEMPORARY FOV Convert horizontal FOV to vertical FOV [ REMOVE LATER ]
    float fovY = 2.0f * atanf(tanf(m_fovDegrees * 0.5f * DEG2RAD) / aspect) * (180.0f / 3.14159265f);
	///////////////////////////////////////
	
	
    return Matrix4x4_f::Perspective(fovY, aspect, m_zNear, m_zFar);
}
