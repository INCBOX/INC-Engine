// player.h
#pragma once
#include "mathlib/vector3_f.h"
#include "mathlib/camera_f.h"
#include "input.h"
#include "movementphysics.h"

class Player {
public:
    Player();

    void Update(float dt, const Input& input);
    Camera& GetCamera() { return m_Camera; }
	
	MovementPhysics m_Movement;

private:
    Vec3_f m_Position;
    Vec3_f m_Velocity;
    Camera m_Camera;

    float m_PlayerHeight = 1.8f; // player eye height in meters

    // Movement speeds
    float m_MoveSpeed = 5.0f; // meters per second
    float m_MouseSensitivity = 0.1f;

    // Other player state like jump, gravity etc can go here
};
