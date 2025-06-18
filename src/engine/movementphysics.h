#pragma once
#include "mathlib/vector.h"
#include "input.h"

class MovementPhysics {
	friend class Player;  // Player can access MovementPhysics private members
	
public:
    MovementPhysics();

    void Update(float dt, const Input& input, Vector& position, Vector& velocity, const Vector& forward, const Vector& right, bool onGround);

    void SetSpeed(float speed) { m_Speed = speed; }
    void SetOnGround(bool value) { m_OnGround = value; }

private:
    void ApplyFriction(float dt, Vector& velocity);
    void ApplyAcceleration(float dt, const Vector& wishDir, float wishSpeed, Vector& velocity);
    void ApplyGravity(float dt, Vector& velocity);
    void HandleJump(Vector& velocity);

    float m_Speed;
    float m_JumpForce;
    float m_Gravity;
    float m_Friction;
    float m_Accel;
    float m_AirAccel;

    bool m_OnGround;
    bool m_JumpPressedLastFrame;
};