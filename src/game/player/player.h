#pragma once
#include "mathlib/vector.h"
#include "mathlib/qangle.h"

class CPlayer {
public:
    Vector m_vecOrigin;
    QAngle m_angView;

    CPlayer();

    void Spawn();
    void Think(float dt);
    void MoveForward(float units);
    void Turn(float yawDelta);
};