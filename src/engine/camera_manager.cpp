#include "camera_manager.h"

CameraManager::CameraManager()
    : activePrecision(CameraPrecision::Float) // default to float camera
{
    // Optionally initialize cameras here
}

void CameraManager::Update(float dt, const Uint8* keystate, int mouseDX, int mouseDY)
{
    switch (activePrecision)
    {
        case CameraPrecision::Float:
            m_Camera_f.Update(dt, keystate, mouseDX, mouseDY);
            break;
        case CameraPrecision::Double:
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
    activePrecision = precision;
}

CameraPrecision CameraManager::GetPrecision() const
{
    return activePrecision;
}


void CameraManager::UpdateRotationOnly(float dt, int mouseDX, int mouseDY)
{
    switch (activePrecision)
    {
        case CameraPrecision::Float:
            m_Camera_f.UpdateRotationOnly(dt, mouseDX, mouseDY);
            break;
        case CameraPrecision::Double:
            m_Camera_d.UpdateRotationOnly(dt, mouseDX, mouseDY);
            break;
    }
}

Matrix4x4_f CameraManager::GetLocalViewMatrix() const
{
    Vector3_d cameraWorld = m_Camera_d.GetPosition();
    Vector3_f localOrigin(0, 0, 0);
    Vector3_f cameraOffset = Vector3_f(
        float(cameraWorld.x - m_WorldOrigin.x),
        float(cameraWorld.y - m_WorldOrigin.y),
        float(cameraWorld.z - m_WorldOrigin.z)
    );

    return Matrix4x4_f::Translate(-cameraOffset) * m_Camera_f.GetViewMatrix();
}

// Floating Origin — Future Additions - periodically reset the floating origin:
void CameraManager::ApplyFloatingOrigin(Vector3_d newOrigin)
{
    m_WorldOrigin = newOrigin;
    // Shift all world entities by -newOrigin
}