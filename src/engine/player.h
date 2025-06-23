// player.h
#pragma once
#include "mathlib/vector3_f.h"
#include "mathlib/vector3_d.h"
#include "mathlib/camera_f.h"
#include "input.h"
#include "movementphysics.h"

class Player {
public:
    Player();

    void Update(float dt, const Input& input);
    Camera_f& GetCamera_f() { return m_Camera_f; }
	
	MovementPhysics m_Movement;

private:
    Vector3_f m_Position;      // float precision position
    Vector3_f m_Velocity;      // float precision velocity

    Vector3_d m_Position_d;    // double precision position
    Vector3_d m_Velocity_d;    // double precision velocity

	Camera_f m_Camera_f;

    float m_PlayerHeight = 1.8f; // player eye height in meters

    // Movement speeds
    float m_MoveSpeed = 5.0f; // meters per second
    float m_MouseSensitivity = 0.1f;

    // Other player state like jump, gravity etc can go here
};
