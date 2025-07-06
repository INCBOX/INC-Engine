#include "player.h"
#include "camera_manager.h"

extern CameraManager g_CameraManager; // Defined somewhere globally

Player::Player()
    : m_Position(0, 0, 0),
      m_Velocity(0, 0, 0),
      m_Position_d(0.0, 0.0, 0.0),
      m_Velocity_d(0.0, 0.0, 0.0),
      m_PlayerHeight(1.8f),
      m_MouseSensitivity(0.1f)
{
    // Set initial camera position for float camera by default
    g_CameraManager.GetCamera_f().SetPosition(m_Position + Vector3_f(0, m_PlayerHeight, 0));
}

void Player::Update(float dt, const Input& input)
{
    auto& cam_d = g_CameraManager.GetCamera_d();

    Vector3_d fwd_d = cam_d.GetForwardVector();
    Vector3_d right_d = cam_d.GetRightVector();

    Vector3_f fwd_f(float(fwd_d.x), 0.0f, float(fwd_d.z));
    Vector3_f right_f(float(right_d.x), 0.0f, float(right_d.z));

    fwd_f = fwd_f.Normalize();
    right_f = right_f.Normalize();

    Vector3_f pos_f(float(m_Position_d.x), float(m_Position_d.y), float(m_Position_d.z));
    Vector3_f vel_f(float(m_Velocity_d.x), float(m_Velocity_d.y), float(m_Velocity_d.z));

    bool onGround = false;
    if (pos_f.y <= 0.0f)
    {
        pos_f.y = 0.0f;
        vel_f.y = 0.0f;
        onGround = true;
    }

    m_Movement.Update(dt, input, pos_f, vel_f, fwd_f, right_f, onGround);

    m_Position_d = Vector3_d(pos_f.x, pos_f.y, pos_f.z);
    m_Velocity_d = Vector3_d(vel_f.x, vel_f.y, vel_f.z);

    // Only update camera_d position here, NOT yaw/pitch
    Vector3_d eyePos = m_Position_d + Vector3_d(0.0, m_PlayerHeight, 0.0);
    cam_d.SetPosition(eyePos);
}
