
#include "RocketController.h"
#include <SDL2/SDL.h>

static Vec3 position = { 0.0f, 0.0f, 0.0f };
static Vec3 velocity = { 0.0f, 0.0f, 0.0f };
static float pitch = 0.0f, yaw = 0.0f, roll = 0.0f;
static const float thrust = 10.0f;
static const float turnSpeed = 1.5f;

void InitRocket() {
    position = { 0.0f, 0.0f, 20.0f };
    velocity = { 0.0f, 0.0f, 0.0f };
    pitch = yaw = roll = 0.0f;
}

void UpdateRocket(float dt) {
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    // Rotate (WASD for pitch/yaw, QE for roll)
    if (keys[SDL_SCANCODE_W]) pitch += turnSpeed * dt;
    if (keys[SDL_SCANCODE_S]) pitch -= turnSpeed * dt;
    if (keys[SDL_SCANCODE_A]) yaw += turnSpeed * dt;
    if (keys[SDL_SCANCODE_D]) yaw -= turnSpeed * dt;
    if (keys[SDL_SCANCODE_Q]) roll += turnSpeed * dt;
    if (keys[SDL_SCANCODE_E]) roll -= turnSpeed * dt;

    // Build direction vector (forward)
    Vec3 dir = {
        -sinf(yaw) * cosf(pitch),
        sinf(pitch),
        -cosf(yaw) * cosf(pitch)
    };

    if (keys[SDL_SCANCODE_UP]) {
        velocity = velocity + dir * thrust * dt;
    }
    if (keys[SDL_SCANCODE_DOWN]) {
        velocity = velocity - dir * thrust * dt;
    }

    position = position + velocity * dt;
}

Mat4 GetRocketTransform() {
    Mat4 t = Mat4::Translation(position);
    Mat4 r = Mat4::RotationYXZ(yaw, pitch, roll);
    return t * r;
}
