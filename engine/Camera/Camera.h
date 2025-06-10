#pragma once

#include "math.h"

class Camera {
public:
	Vec3 GetFront() const { return Front; }
	Vec3 GetRight() const { return Right; }

    Vec3 Position;
    float Pitch;
    float Yaw;

    Camera(const Vec3& position = Vec3(0, 0, 5), float pitch = 0.0f, float yaw = -90.0f);

    void ProcessMouse(float xoffset, float yoffset, float sensitivity = 0.1f);
    void ProcessKeyboard(const Vec3& direction, float deltaTime, float speed = 3.0f);
    Mat4 GetViewMatrix() const;

private:
    Vec3 Front;
    Vec3 Right;
    Vec3 Up;
    Vec3 WorldUp;

    void UpdateVectors();
};
