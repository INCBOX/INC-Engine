// FPSController.cpp
#include "FPS_controller.h"
#include "mathlib/constants.h"
using namespace mathlib;

// ---- Global Variables ----
float gCameraSpeed = 5.0f;

FPSController::FPSController()
    : position(0.0f, 0.0f, 3.0f),  // ← move back 3 units so you can see forward
      velocity(0.0f, 0.0f, 0.0f), pitch(0.0f), yaw(0.0f),
      moveSpeed(6.4f), mouseSensitivity(0.1f), airAcceleration(2.0f), groundAcceleration(10.0f),
      onGround(true) {}


FPSController::FPSController(const Vec3& startPosition)
    : position(startPosition), velocity(0.0f, 0.0f, 0.0f), pitch(0.0f), yaw(0.0f),
      moveSpeed(6.4f), mouseSensitivity(0.1f), airAcceleration(2.0f), groundAcceleration(10.0f),
      onGround(true) {}

void FPSController::HandleMouseMotion(float dx, float dy) {
    yaw += dx * mouseSensitivity;
    pitch -= dy * mouseSensitivity;
    pitch = std::max(-89.0f, std::min(89.0f, pitch));
}

Vec3 FPSController::GetForward() const {
    float radPitch = DegToRad(pitch);
    float radYaw = DegToRad(yaw);
    return Vec3(
        cos(radPitch) * cos(radYaw),
        sin(radPitch),
        cos(radPitch) * sin(radYaw)
    ).Normalized();
}

Vec3 FPSController::GetRight() const {
    return GetForward().Cross(Vec3(0, 1, 0)).Normalized();
}

void FPSController::Accelerate(const Vec3& wishDir, float wishSpeed, float accel, float dt) {
    float currentSpeed = velocity.Dot(wishDir);
    float addSpeed = wishSpeed - currentSpeed;
    if (addSpeed <= 0.0f) return;
    float accelSpeed = accel * wishSpeed * dt;
    if (accelSpeed > addSpeed) accelSpeed = addSpeed;
    velocity += wishDir * accelSpeed;
}

void FPSController::Update(float dt, const unsigned char* keys) {
    Vec3 forward = GetForward();
    Vec3 right = GetRight();
    Vec3 moveDir(0.0f, 0.0f, 0.0f);

    if (keys['W']) moveDir += forward;
    if (keys['S']) moveDir -= forward;
    if (keys['A']) moveDir -= right;
    if (keys['D']) moveDir += right;

    moveDir = moveDir.Normalized();
    float wishSpeed = moveSpeed;

    if (onGround)
        Accelerate(moveDir, wishSpeed, groundAcceleration, dt);
    else
        Accelerate(moveDir, wishSpeed, airAcceleration, dt);

    position += velocity * dt;
}

Mat4 FPSController::GetViewMatrix() const {
    return Mat4::LookAt(position, position + GetForward(), Vec3(0, 1, 0));
}

Mat4 FPSController::GetProjectionMatrix(float aspect) const {
    return Mat4::Perspective(DegToRad(70.0f), aspect, 0.1f, 1000.0f);
}

Vec3 FPSController::GetPosition() const {
    return position;
}
