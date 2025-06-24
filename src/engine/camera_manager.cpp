#include "camera_manager.h"
#include "mathlib/precision_convert.h"
#include <iostream>

CameraManager::CameraManager()
    : activePrecision(CameraPrecision::Float) // default to float camera
{
    std::cout << "[CameraManager] Initialized with Float precision\n";
}

void CameraManager::Update(float dt, const Uint8* keystate, int mouseDX, int mouseDY)
{
    std::cout << "[CameraManager::Update] dt=" << dt << " mouseDX=" << mouseDX << " mouseDY=" << mouseDY << "\n";
    switch (activePrecision)
    {
        case CameraPrecision::Float:
            std::cout << "[CameraManager] Using Float precision camera\n";
            m_Camera_f.Update(dt, keystate, mouseDX, mouseDY);
            break;
        case CameraPrecision::Double:
            std::cout << "[CameraManager] Using Double precision camera\n";
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
    std::cout << "[CameraManager] Precision changed from " 
              << (activePrecision == CameraPrecision::Float ? "Float" : "Double") 
              << " to " 
              << (precision == CameraPrecision::Float ? "Float" : "Double") << "\n";

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
    std::cout << "[CameraManager] mouseDX: " << mouseDX << ", mouseDY: " << mouseDY << "\n";
    std::cout << "[CameraManager] yawDelta: " << yawDelta << ", pitchDelta: " << pitchDelta << "\n";

    switch (activePrecision) {
        case CameraPrecision::Float: {
            float yaw = m_Camera_f.GetYaw();
            float pitch = m_Camera_f.GetPitch();
            std::cout << "[CameraManager::Float] Before update yaw: " << yaw << ", pitch: " << pitch << "\n";
            yaw += yawDelta;
            pitch += pitchDelta;
            m_Camera_f.SetYaw(yaw);
            m_Camera_f.SetPitch(pitch);
            m_Camera_f.ClampPitch(-89.9f, 89.9f);
            m_Camera_f.UpdateOrientation();
            std::cout << "[CameraManager::Float] After update yaw: " << m_Camera_f.GetYaw() 
                      << ", pitch: " << m_Camera_f.GetPitch() << "\n";
            break;
        }

        case CameraPrecision::Double:
        {
            double yaw = m_Camera_d.GetYaw();
            double pitch = m_Camera_d.GetPitch();
            std::cout << "[CameraManager::Double] Before update yaw: " << yaw << ", pitch: " << pitch << "\n";
            yaw += static_cast<double>(yawDelta);
            pitch += static_cast<double>(pitchDelta);
            m_Camera_d.SetYaw(yaw);
            m_Camera_d.SetPitch(pitch);
            m_Camera_d.ClampPitch(-89.9, 89.9);
            m_Camera_d.UpdateOrientation();
            std::cout << "[CameraManager::Double] After update yaw: " << m_Camera_d.GetYaw() 
                      << ", pitch: " << m_Camera_d.GetPitch() << "\n";
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
            std::cout << "[CameraManager] Camera position (double): " 
                      << camPos.x << ", " << camPos.y << ", " << camPos.z << "\n";
            Vector3_d localCamPos = camPos - m_WorldOrigin;
            std::cout << "[CameraManager] Local camera position after floating origin: " 
                      << localCamPos.x << ", " << localCamPos.y << ", " << localCamPos.z << "\n";
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
    std::cout << "[CameraManager] Applying floating origin shift: " 
              << newOrigin.x << ", " << newOrigin.y << ", " << newOrigin.z << "\n";
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
        std::cout << "[CameraManager] Floating origin shift triggered, delta = ("
                  << delta.x << ", " << delta.y << ", " << delta.z << ")\n";

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

    std::cout << "[CameraManager] Floating origin shifted to (" 
              << m_WorldOrigin.x << ", " << m_WorldOrigin.y << ", " << m_WorldOrigin.z << ")\n";
}
Vector3_d CameraManager::GetWorldOrigin() const
{
    return m_WorldOrigin;
}
