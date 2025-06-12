#include "public/vphysics/IPhysicsObject.h"
#include <iostream>

class SimplePhysicsObject : public IPhysicsObject {
    Vector m_pos;

public:
    SimplePhysicsObject() : m_pos(0,0,0) {}

    void ApplyForce(const Vector& force) override {
        m_pos += force; // naive physics
        std::cout << "[Physics] Force applied, new pos: " << m_pos.x << ", " << m_pos.y << ", " << m_pos.z << std::endl;
    }

    void GetPosition(Vector& outPos) const override {
        outPos = m_pos;
    }

    void SetPosition(const Vector& newPos) override {
        m_pos = newPos;
    }
};