#pragma once
#include "mathlib/vec3.h"
#include "mathlib/mat4.h"

class Level_TestRoom {
public:
    void Init();
    void Update(float dt);
    void Render(const mathlib::Mat4& viewProj);
    mathlib::Vec3 GetPlayerSpawn() const;
};
