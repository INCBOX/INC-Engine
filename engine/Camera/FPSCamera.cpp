#include "FPSCamera.h"
#include <cmath>

static constexpr float MOVE_SPEED = 5.0f;
static constexpr float JUMP_FORCE = 8.0f;
static constexpr float GRAVITY = 20.0f;

FPSCamera::FPSCamera()
    : Position{0.0f, 1.0f, 3.0f}, Velocity{0.0f, 0.0f, 0.0f},
      Yaw(-90.0f), Pitch(0.0f), IsGrounded(true)
{
    updateVectors();
}

void FPSCamera::Update(float deltaTime)
{
    if (!IsGrounded) {
        Velocity.y -= GRAVITY * deltaTime;
    }

    Position += Velocity * deltaTime;

    // Simple ground collision
    if (Position.y <= 1.0f) {
        Position.y = 1.0f;
        Velocity.y = 0.0f;
        IsGrounded = true;
    }
}

void FPSCamera::ProcessInput(const Uint8* keys, float deltaTime)
{
    Vec3 move(0, 0, 0);
    float velocity = MOVE_SPEED * deltaTime;

    if (keys[SDL_SCANCODE_W]) move += Front;
    if (keys[SDL_SCANCODE_S]) move -= Front;
    if (keys[SDL_SCANCODE_A]) move -= Right;
    if (keys[SDL_SCANCODE_D]) move += Right;

    move.y = 0.0f; // Lock movement to XZ
    if (move.length() > 0.0f)
        move = move.normalized();

    Position += move * velocity;

    if (keys[SDL_SCANCODE_SPACE] && IsGrounded) {
        Velocity.y = JUMP_FORCE;
        IsGrounded = false;
    }
}

Mat4 FPSCamera::GetViewMatrix() const
{
    return Mat4::lookAt(Position, Position + Front, Up);
}

void FPSCamera::updateVectors()
{
    float yawRad = radians(Yaw);
    float pitchRad = radians(Pitch);

    Vec3 front;
    front.x = cosf(pitchRad) * cosf(yawRad);
    front.y = sinf(pitchRad);
    front.z = cosf(pitchRad) * sinf(yawRad);

    Front = front.normalized();
    Right = Front.cross(Vec3{0.0f, 1.0f, 0.0f}).normalized();
    Up    = Right.cross(Front).normalized();
}

void FPSCamera::ProcessMouseMovement(float xoffset, float yoffset)
{
    // Invert Y for normal FPS feel
    yoffset = -yoffset;

    // Apply smoothing (CS2 style)
    SmoothedX = SmoothedX * MouseSmoothing + xoffset * (1.0f - MouseSmoothing);
    SmoothedY = SmoothedY * MouseSmoothing + yoffset * (1.0f - MouseSmoothing);

    Yaw   += SmoothedX * MouseSensitivity;
    Pitch += SmoothedY * MouseSensitivity;

    // Clamp pitch to prevent flipping
    if (Pitch > 89.0f)  Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    updateVectors();
}
