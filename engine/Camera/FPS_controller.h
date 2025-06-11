// FPSController.h
#pragma once

#include "mathlib/Vec3.h"
#include "mathlib/Mat4.h"

class FPSController {
public:
    FPSController();
    FPSController(const mathlib::Vec3& startPosition);

    void HandleMouseMotion(float dx, float dy);
    void Update(float dt, const unsigned char* keys);

    mathlib::Mat4 GetViewMatrix() const;
    mathlib::Mat4 GetProjectionMatrix(float aspect) const;
    mathlib::Vec3 GetPosition() const;

private:
    mathlib::Vec3 position;
    mathlib::Vec3 velocity;
    float pitch;
    float yaw;

    float moveSpeed;
    float mouseSensitivity;
    float airAcceleration;
    float groundAcceleration;

    bool onGround;

    mathlib::Vec3 GetForward() const;
    mathlib::Vec3 GetRight() const;
    void Accelerate(const mathlib::Vec3& wishDir, float wishSpeed, float accel, float dt);
};
