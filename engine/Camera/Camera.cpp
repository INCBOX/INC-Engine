#include "Camera.h"

Camera::Camera(const Vec3& position, float pitch, float yaw)
    : Position(position), Pitch(pitch), Yaw(yaw), WorldUp(0, 1, 0) {
    UpdateVectors();
}

void Camera::ProcessMouse(float xoffset, float yoffset, float sensitivity) {
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // Clamp pitch to avoid flip
    if (Pitch > 89.0f) Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    UpdateVectors();
}

void Camera::ProcessKeyboard(const Vec3& direction, float deltaTime, float speed) {
    float velocity = speed * deltaTime;
    Position += direction * velocity;
}

void Camera::UpdateVectors() {
    float yawRad = radians(Yaw);
    float pitchRad = radians(Pitch);

    Front.x = cos(yawRad) * cos(pitchRad);
    Front.y = sin(pitchRad);
    Front.z = sin(yawRad) * cos(pitchRad);
    Front = Front.normalized();

    Right = Front.cross(WorldUp).normalized();
    Up = Right.cross(Front).normalized();
}

Mat4 Camera::GetViewMatrix() const {
    return Mat4::lookAt(Position, Position + Front, Up);
}