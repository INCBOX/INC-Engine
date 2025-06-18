#pragma once
#include "mathlib/vector.h"
#include "input.h"

class MovementController {
public:
    MovementController();

    void Update(float dt, const Input& input, Vector& position, const Vector& forward, const Vector& right);

    void SetSpeed(float speed) { m_Speed = speed; }

private:
    float m_Speed;
};