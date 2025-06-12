#pragma once
#include "mathlib/vector.h"

class IPhysicsObject {
public:
    virtual ~IPhysicsObject() {}
    virtual void ApplyForce(const Vector& force) = 0;
    virtual void GetPosition(Vector& outPos) const = 0;
    virtual void SetPosition(const Vector& newPos) = 0;
};