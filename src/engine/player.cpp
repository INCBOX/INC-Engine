// Player.cpp
#include "mathlib/vector3_f.h"
#include "player.h"
#include <SDL.h>


// m_Movement.m_OnGround = true;  
// m_OnGround = true; 

Player::Player() : m_Position(0, 0, 0), m_Velocity(0, 0, 0) {
    m_Camera.SetPosition(m_Position + Vec3_f(0, m_PlayerHeight, 0));
    m_Movement.m_OnGround = true; // Initialize here (Player is friend of MovementPhysics)
}

void Player::Update(float dt, const Input& input) {
    // For now, set on ground status before update
    m_Movement.m_OnGround = true;  // or update properly with collision later

    // Handle rotation from mouse deltas
    int mouseDX = input.GetMouseDeltaX();
    int mouseDY = input.GetMouseDeltaY();

    m_Camera.AddYaw(mouseDX * m_MouseSensitivity);
    m_Camera.AddPitch(-mouseDY * m_MouseSensitivity); // invert Y if desired

    // Clamp pitch to prevent flipping
    m_Camera.ClampPitch(-89.9f, 89.9f);

    // Calculate forward/right vectors
    Vec3_f forward = m_Camera.GetForwardVector();
    Vec3_f right = m_Camera.GetRightVector();

    // Apply movement input, passing the m_OnGround state from MovementPhysics
    m_Movement.Update(dt, input, m_Position, m_Velocity, forward, right, m_Movement.m_OnGround);

    // Update camera position to match player head
    m_Camera.SetPosition(m_Position + Vec3_f(0, m_PlayerHeight, 0));

    m_Camera.UpdateOrientation(); // PLAYER CONTROLS CAMERA NOW
}
