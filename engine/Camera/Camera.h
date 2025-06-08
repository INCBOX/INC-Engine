#ifndef CAMERA_H
#define CAMERA_H

#pragma once
#include "../Math/Math.h"

class Camera {
public:
    Vec3 Position;
    Vec3 Front;
    Vec3 Up;
    Vec3 Right;
    Vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    Camera(Vec3 position = Vec3(0.0f, 0.0f, 3.0f), Vec3 up = Vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);

    void ProcessKeyboard(int direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);

    Mat4 GetViewMatrix();

private:
    void updateCameraVectors();
};

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

#endif // CAMERA_H