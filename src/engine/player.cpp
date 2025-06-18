// Player.cpp
#include "player.h"
#include <SDL.h>

Player::Player() : m_Position(0, 0, 0), m_Velocity(0, 0, 0) {
    m_Camera.SetPosition(m_Position + Vector(0, m_PlayerHeight, 0));
}

void Player::Update(float dt, const Input& input) {
    // Handle rotation from mouse deltas
    int mouseDX = input.GetMouseDeltaX();
    int mouseDY = input.GetMouseDeltaY();

    m_Camera.AddYaw(mouseDX * m_MouseSensitivity);
    m_Camera.AddPitch(-mouseDY * m_MouseSensitivity); // invert Y if desired

    // Clamp pitch to prevent flipping
    m_Camera.ClampPitch(-89.9f, 89.9f); // Prevent flipping

    // Calculate forward/right/flat vectors
    Vector forward = m_Camera.GetForwardVector();
    Vector right = m_Camera.GetRightVector();

    // Apply movement input
    m_Movement.Update(dt, input, m_Position, forward, right);
	
    // Update camera position to match player head
    m_Camera.SetPosition(m_Position + Vector(0, m_PlayerHeight, 0));
	
	m_Camera.UpdateOrientation(); // PLAYER CONTROLS CAMERA NOW
}