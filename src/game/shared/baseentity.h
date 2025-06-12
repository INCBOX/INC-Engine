#pragma once
#include "mathlib/vector.h"

class CBaseEntity {
public:
    Vector m_vecOrigin;

    CBaseEntity() : m_vecOrigin(0, 0, 0) {}
    virtual ~CBaseEntity() {}

    virtual void Spawn() = 0;
    virtual void Update(float dt) = 0;
};