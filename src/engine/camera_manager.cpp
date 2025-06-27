#include "camera_manager.h"
#include "mathlib/precision_convert.h"

CameraManager::CameraManager()
    : activePrecision(CameraPrecision::Float) // default to float camera
{
    // Removed debug print
}

void CameraManager::Update(float dt, const Uint8* keystate, int mouseDX, int mouseDY)
{
    // Removed debug print
    switch (activePrecision)
    {
        case CameraPrecision::Float:
            // Removed debug print
            m_Camera_f.Update(dt, keystate, mouseDX, mouseDY);
            break;
        case CameraPrecision::Double:
            // Removed debug print
            m_Camera_d.Update(dt, keystate, mouseDX, mouseDY);
            break;
    }
}

Matrix4x4_f CameraManager::GetViewMatrix_f() const
{
    return m_Camera_f.GetViewMatrix();
}

Matrix4x4_f CameraManager::GetProjectionMatrix_f(float aspect) const {
    return m_Camera_f.GetProjectionMatrix(aspect);
}

Matrix4x4_d CameraManager::GetViewMatrix_d() const
{
    return m_Camera_d.GetViewMatrix();
}

Matrix4x4_d CameraManager::GetProjectionMatrix_d(double aspect) const
{
    return m_Camera_d.GetProjectionMatrix(aspect);
}

void CameraManager::SetPrecision(CameraPrecision precision)
{
    // Removed debug print
    activePrecision = precision;
}

CameraPrecision CameraManager::GetPrecision() const
{
    return activePrecision;
}

void CameraManager::UpdateRotationOnly(float dt, int mouseDX, int mouseDY) {
    constexpr float sensitivity = 0.1f;

    float yawDelta = static_cast<float>(mouseDX) * sensitivity;
    float pitchDelta = static_cast<float>(-mouseDY) * sensitivity; // Inverted Y
    // Removed debug prints

    switch (activePrecision) {
        case CameraPrecision::Float: {
            float yaw = m_Camera_f.GetYaw();
            float pitch = m_Camera_f.GetPitch();
            // Removed debug print
            yaw += yawDelta;
            pitch += pitchDelta;
            m_Camera_f.SetYaw(yaw);
            m_Camera_f.SetPitch(pitch);
            m_Camera_f.ClampPitch(-89.9f, 89.9f);
            m_Camera_f.UpdateOrientation();
            // Removed debug print
            break;
        }

        case CameraPrecision::Double:
        {
            double yaw = m_Camera_d.GetYaw();
            double pitch = m_Camera_d.GetPitch();
            // Removed debug print
            yaw += static_cast<double>(yawDelta);
            pitch += static_cast<double>(pitchDelta);
            m_Camera_d.SetYaw(yaw);
            m_Camera_d.SetPitch(pitch);
            m_Camera_d.ClampPitch(-89.9, 89.9);
            m_Camera_d.UpdateOrientation();
            // Removed debug print
            break;
        }
    }
}

Matrix4x4_f CameraManager::GetLocalViewMatrix() const
{
    switch (activePrecision)
    {
        case CameraPrecision::Float:
            return m_Camera_f.GetViewMatrix();

        case CameraPrecision::Double:
        {
            Vector3_d camPos = m_Camera_d.GetPosition();
            // Removed debug prints
            Vector3_d localCamPos = camPos - m_WorldOrigin;
            // Removed debug prints
            Camera_d temp = m_Camera_d;
            temp.SetPosition(localCamPos);
            Matrix4x4_d localView_d = temp.GetViewMatrix();
            return precision::ToFloat(localView_d);
        }
    }
    return Matrix4x4_f::Identity();
}

void CameraManager::ApplyFloatingOrigin(Vector3_d newOrigin)
{
    // Removed debug print
    m_WorldOrigin = newOrigin;
    // Shift all world entities by -newOrigin
}

// FLOATING ORIGIN LOGIC:
void CameraManager::UpdateFloatingOrigin(const Vector3_d& playerPosition)
{
    Vector3_d delta = playerPosition - m_WorldOrigin;
    double distSquared = delta.LengthSqr();

    if (distSquared > (FloatingOriginThreshold * FloatingOriginThreshold))
    {
        // Removed debug print

        ApplyFloatingOriginShift(delta);
    }
}

void CameraManager::ApplyFloatingOriginShift(const Vector3_d& delta)
{
    // Shift the world origin
    m_WorldOrigin += delta;

    // Shift Camera_d position by negative delta to keep local space near zero
    Vector3_d camPos = m_Camera_d.GetPosition();
    m_Camera_d.SetPosition(camPos - delta);

    // TODO: Shift all world entities (players, objects, etc) by -delta
    // For now, assume player position will be shifted by caller after this

    // Removed debug print
}

Vector3_d CameraManager::GetWorldOrigin() const
{
    return m_WorldOrigin;
}