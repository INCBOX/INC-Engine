#pragma once
#include "mathlib/camera_f.h"
#include "mathlib/camera_d.h"
#include <SDL.h>

enum class CameraPrecision {
    Float,
    Double
};

class CameraManager {
public:
    CameraManager();

    // Update active camera based on input, dt, mouse, keys, etc
    void Update(float dt, const Uint8* keystate, int mouseDX, int mouseDY);

    // Get current active camera view/projection matrices
    Matrix4x4_f GetViewMatrix_f() const;
    Matrix4x4_f GetProjectionMatrix_f(float aspect) const;

    Matrix4x4_d GetViewMatrix_d() const;
    Matrix4x4_d GetProjectionMatrix_d(double aspect) const;

    // Switch active camera precision mode
    void SetPrecision(CameraPrecision precision);
    CameraPrecision GetPrecision() const;

    // Access underlying cameras if needed
	Camera_f& GetCamera_f() { return m_Camera_f; }
	Camera_d& GetCamera_d() { return m_Camera_d; }
	
	void UpdateRotationOnly(float dt, int mouseDX, int mouseDY);
	
	Matrix4x4_f GetLocalViewMatrix() const;
	
	// IMPLEMENTED LATER!
	void ApplyFloatingOrigin(Vector3_d newOrigin);
	
	
	// FLOATING ORIGIN LOGIC
	Vector3_d GetWorldOrigin() const;
    void UpdateFloatingOrigin(const Vector3_d& playerPosition);
    void ApplyFloatingOriginShift(const Vector3_d& delta);


private:
    void ClampPitch(float& pitch, float minPitch, float maxPitch);
    Vector3_f CalculateForwardVector(float yawDeg, float pitchDeg);
    Vector3_f CalculateRightVector(float yawDeg);
	
    Camera_f m_Camera_f;    // float precision camera
    Camera_d m_Camera_d;    // double precision camera
	
	// FLOATING ORIGIN LOGIC
	Vector3_d m_WorldOrigin = Vector3_d(0.0, 0.0, 0.0);
	const double FloatingOriginThreshold = 1000.0; // meters; tune as needed

	//////////
	
    CameraPrecision activePrecision;

};