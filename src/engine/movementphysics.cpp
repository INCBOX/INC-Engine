#include "movementphysics.h"
#include <SDL.h>

MovementPhysics::MovementPhysics()
    : m_Speed(10.0f),
      m_JumpForce(5.0f),
      m_Gravity(9.81f),
      m_Friction(6.0f),
      m_Accel(10.0f),
      m_AirAccel(2.0f),
      m_OnGround(false),
      m_JumpPressedLastFrame(false)
{}

void MovementPhysics::Update(float dt, const Input& input, Vector& position, Vector& velocity, const Vector& forward, const Vector& right, bool onGround)
{
    m_OnGround = onGround;
    const Uint8* keys = input.GetKeyState();

    Vector wishDir(0, 0, 0);
    if (keys[SDL_SCANCODE_W]) wishDir += Vector(forward.x, 0, forward.z);
    if (keys[SDL_SCANCODE_S]) wishDir -= Vector(forward.x, 0, forward.z);
    if (keys[SDL_SCANCODE_A]) wishDir -= right;
    if (keys[SDL_SCANCODE_D]) wishDir += right;

    if (wishDir.Length() > 0.01f)
        wishDir = wishDir.Normalize();

    float wishSpeed = m_Speed;

    if (m_OnGround)
    {
        ApplyFriction(dt, velocity);
        ApplyAcceleration(dt, wishDir, wishSpeed, velocity);

        // Jumping
        bool jumpPressed = keys[SDL_SCANCODE_SPACE];
        if (jumpPressed && !m_JumpPressedLastFrame)
            HandleJump(velocity);

        m_JumpPressedLastFrame = jumpPressed;
    }
    else
    {
        ApplyGravity(dt, velocity);
        ApplyAcceleration(dt, wishDir, wishSpeed, velocity); // air control
    }

    // Apply final velocity
    position += velocity * dt;
}

void MovementPhysics::ApplyFriction(float dt, Vector& velocity)
{
    float speed = velocity.Length();
    if (speed < 0.01f) return;

    float drop = speed * m_Friction * dt;
    float newSpeed = speed - drop;
    if (newSpeed < 0.0f) newSpeed = 0.0f;

    velocity = velocity.Normalize() * newSpeed;
}

void MovementPhysics::ApplyAcceleration(float dt, const Vector& wishDir, float wishSpeed, Vector& velocity)
{
    float currentSpeed = velocity.Dot(wishDir);
    float addSpeed = wishSpeed - currentSpeed;
    if (addSpeed <= 0) return;

    float accel = m_OnGround ? m_Accel : m_AirAccel;
    float accelSpeed = accel * wishSpeed * dt;

    if (accelSpeed > addSpeed)
        accelSpeed = addSpeed;

    velocity += wishDir * accelSpeed;
}

void MovementPhysics::ApplyGravity(float dt, Vector& velocity)
{
    velocity.y -= m_Gravity * dt;
}

void MovementPhysics::HandleJump(Vector& velocity)
{
    velocity.y = m_JumpForce;
}