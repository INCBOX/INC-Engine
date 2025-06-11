#pragma once
#include "mathlib/vec3.h"
#include "mathlib/mat4.h"

#include <SDL2/SDL.h>

class CSGOPlayer {
public:
    void Init(const mathlib::Vec3& spawnPos);
    void HandleInput(const Uint8* keys, int mouseDX, int mouseDY, float dt);
    mathlib::Mat4 GetViewMatrix() const;
    mathlib::Vec3 GetPosition() const;

private:
    mathlib::Vec3 position;
    mathlib::Vec3 velocity;
    float yaw = 0, pitch = 0;
    bool isGrounded = false;
    const float groundHeight = 1.0f;
};
