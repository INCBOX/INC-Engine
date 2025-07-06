#pragma once
#include "mathlib/vector3_f.h"
#include "input.h"

class MovementPhysics {
	friend class Player;  // Player can access MovementPhysics private members
	
public:
    MovementPhysics();

    void Update(float dt, const Input& input, Vector3_f& position, Vector3_f& velocity, const Vector3_f& forward, const Vector3_f& right, bool onGround);

    void SetSpeed(float speed) { m_Speed = speed; }
    void SetOnGround(bool value) { m_OnGround = value; }

private:
    void ApplyFriction(float dt, Vector3_f& velocity);
    void ApplyAcceleration(float dt, const Vector3_f& wishDir, float wishSpeed, Vector3_f& velocity);
    void ApplyGravity(float dt, Vector3_f& velocity);
    void HandleJump(Vector3_f& velocity);

    float m_Speed;
    float m_JumpForce;
    float m_Gravity;
    float m_Friction;
    float m_Accel;
    float m_AirAccel;

    bool m_OnGround;
    bool m_JumpPressedLastFrame;
};