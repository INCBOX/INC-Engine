
#include "CSGOPlayer.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

#include "mathlib/Utils.h"

using namespace mathlib;
using mathlib::Clamp;
using mathlib::ToRadians;

void CSGOPlayer::Init(const Vec3& spawnPos) {
    position = spawnPos;
    velocity = Vec3(0, 0, 0);
	yaw = 90.0f;   // Face +X (toward your room)
	pitch = 0.0f;  // Horizontal
    isGrounded = false;
}

void CSGOPlayer::HandleInput(const Uint8* keys, int mouseDX, int mouseDY, float dt) {
    const float mouseSensitivity = 0.1f;
    const float moveSpeed = isGrounded ? 300.0f : 100.0f;
    const float accel = isGrounded ? 50.0f : 20.0f;

    yaw   += mouseDX * mouseSensitivity;
    pitch -= mouseDY * mouseSensitivity;
    pitch = Clamp(pitch, -89.9f, 89.9f);

    Vec3 forward = Vec3(
        cosf(ToRadians(yaw)) * cosf(ToRadians(pitch)),
        sinf(ToRadians(pitch)),
        sinf(ToRadians(yaw)) * cosf(ToRadians(pitch))
    ).Normalized();
    Vec3 right = forward.Cross(Vec3(0, 1, 0)).Normalized();

    Vec3 wishDir(0, 0, 0);
    if (keys[SDL_SCANCODE_W]) wishDir += forward;
    if (keys[SDL_SCANCODE_S]) wishDir -= forward;
    if (keys[SDL_SCANCODE_A]) wishDir -= right;
    if (keys[SDL_SCANCODE_D]) wishDir += right;

    if (wishDir.LengthSq() > 0)
        wishDir = wishDir.Normalized();

    // Acceleration
    velocity += wishDir * accel * dt;

    // Cap speed (simulate friction)
    if (velocity.Length() > moveSpeed)
        velocity = velocity.Normalized() * moveSpeed;

    // Gravity
    velocity.y -= 980.0f * dt; // gravity

    // Jump
    if (keys[SDL_SCANCODE_SPACE] && isGrounded) {
        velocity.y = 300.0f;
        isGrounded = false;
    }

    // Crouch jump modifier
    if (keys[SDL_SCANCODE_LCTRL]) {
        velocity.y -= 50.0f * dt; // crouch speed
    }

    // Update position
    position += velocity * dt;

    // Simplified ground collision
    if (position.y <= groundHeight) {
        position.y = groundHeight;
        velocity.y = 0;
        isGrounded = true;
    }
}

Mat4 CSGOPlayer::GetViewMatrix() const {
    Vec3 lookDir = Vec3(
        cosf(ToRadians(yaw)) * cosf(ToRadians(pitch)),
        sinf(ToRadians(pitch)),
        sinf(ToRadians(yaw)) * cosf(ToRadians(pitch))
    );
    return Mat4::LookAt(position, position + lookDir, Vec3(0, 1, 0));
}

Vec3 CSGOPlayer::GetPosition() const {
    return position;
}
