#include "Camera.h"
#include <cmath>

Camera::Camera(Vec3 position, Vec3 up, float yaw, float pitch)
    : Front(Vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(2.5f), MouseSensitivity(0.1f), Zoom(45.0f)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

void Camera::ProcessKeyboard(int direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

	if (constrainPitch) {
		if (Pitch > 89.0f) Pitch = 89.0f;
		if (Pitch < -89.0f) Pitch = -89.0f;
	}

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

Mat4 Camera::GetViewMatrix()
{
    return Mat4::lookAt(Position, Position + Front, Up);
}

void Camera::updateCameraVectors()
{
    Vec3 front;
    front.x = std::cos(radians(Yaw)) * std::cos(radians(Pitch));
    front.y = std::sin(radians(Pitch));
    front.z = std::sin(radians(Yaw)) * std::cos(radians(Pitch));
    Front = front.normalized();

    Right = Front.cross(WorldUp).normalized();
    Up = Right.cross(Front).normalized();
}