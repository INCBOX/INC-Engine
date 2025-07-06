#pragma once
#include "mathlib/vector3_f.h"
#include "mathlib/matrix4x4_f.h"
#include <SDL2/SDL.h> // SHOULD BE REMOVED AND UPDATED WITHOUT IT. Uint8

// Camera class for handling position, rotation, and view matrix calculations.
// Supports mouse look and WASD movement on a flat plane.
class Camera_f
{
public:
    Camera_f();

    // Update camera orientation and position based on input
    void Update(float deltaTime, const Uint8* keystate, int mouseDX, int mouseDY);
	
    // Get the view matrix for rendering
    Matrix4x4_f GetViewMatrix() const;
	
	///PLAYER //////////////////////////////////////
    // Set the camera's absolute position
    void SetPosition(const Vector3_f& pos);

    // Get the camera's current position
    Vector3_f GetPosition() const;

    // Add to yaw (rotation around vertical axis)
    void AddYaw(float degrees);

    // Add to pitch (rotation around right axis)
    void AddPitch(float degrees);

    // Clamp pitch to prevent flipping over
    void ClampPitch(float minDegrees, float maxDegrees);

    // Get the normalized forward direction vector the camera is facing
    Vector3_f GetForwardVector() const;

    // Get the normalized right direction vector relative to the camera's forward vector
    Vector3_f GetRightVector() const;
	

	
	// CAMERA MVP SETUP
	Matrix4x4_f GetProjectionMatrix(float aspectRatio) const;

	void UpdateRotationOnly(float deltaTime, int mouseDX, int mouseDY);
	
	// NEW
	float GetYaw() const { return yaw; }
	float GetPitch() const { return pitch; }
	void SetYaw(float y) { yaw = y; }
	void SetPitch(float p) { pitch = p; }
	
	void UpdateOrientation(); // recalculates forward/right/up

private:
    Vector3_f position;       // Camera position in world space
    Vector3_f forward;        // Forward direction vector (normalized)
    float yaw;             // Horizontal angle in degrees (rotation around Y axis)
    float pitch;           // Vertical angle in degrees (rotation around X axis)
    float cameraSpeed;     // Movement speed (units per second)
    float mouseSensitivity;// Mouse sensitivity for looking around (degrees per pixel)
	
    float m_fovDegrees = 120.0f; // Field of view in degrees
    float m_zNear = 0.1f;       // Near clipping plane
    float m_zFar = 1000.0f;     // Far clipping plane
};
