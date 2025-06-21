#pragma once
#include "mathlib/vector3_f.h"
#include "input.h"

class MovementPhysics {
	friend class Player;  // Player can access MovementPhysics private members
	
public:
    MovementPhysics();

    void Update(float dt, const Input& input, Vec3_f& position, Vec3_f& velocity, const Vec3_f& forward, const Vec3_f& right, bool onGround);

    void SetSpeed(float speed) { m_Speed = speed; }
    void SetOnGround(bool value) { m_OnGround = value; }

private:
    void ApplyFriction(float dt, Vec3_f& velocity);
    void ApplyAcceleration(float dt, const Vec3_f& wishDir, float wishSpeed, Vec3_f& velocity);
    void ApplyGravity(float dt, Vec3_f& velocity);
    void HandleJump(Vec3_f& velocity);

    float m_Speed;
    float m_JumpForce;
    float m_Gravity;
    float m_Friction;
    float m_Accel;
    float m_AirAccel;

    bool m_OnGround;
    bool m_JumpPressedLastFrame;
};