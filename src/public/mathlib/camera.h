#pragma once
#include "mathlib/vector.h"
#include "mathlib/matrix.h"
#include <SDL.h>

// Camera class for handling position, rotation, and view matrix calculations.
// Supports mouse look and WASD movement on a flat plane.
class Camera
{
public:
    Camera();

    // Update camera orientation and position based on input
    void Update(float deltaTime, const Uint8* keystate, int mouseDX, int mouseDY);
	
    // Get the view matrix for rendering
    Mat4 GetViewMatrix() const;
	
	///PLAYER //////////////////////////////////////
    // Set the camera's absolute position
    void SetPosition(const Vec3& pos);

    // Get the camera's current position
    Vec3 GetPosition() const;

    // Add to yaw (rotation around vertical axis)
    void AddYaw(float degrees);

    // Add to pitch (rotation around right axis)
    void AddPitch(float degrees);

    // Clamp pitch to prevent flipping over
    void ClampPitch(float minDegrees, float maxDegrees);

    // Get the normalized forward direction vector the camera is facing
    Vec3 GetForwardVector() const;

    // Get the normalized right direction vector relative to the camera's forward vector
    Vec3 GetRightVector() const;
	
	// PLAYER CONTROLS CAMERA
	void UpdateOrientation(); // orientation only
	///////////////////////////////////////////////
	
	// CAMERA MVP SETUP
	Mat4 GetProjectionMatrix(float aspectRatio) const;

private:
    Vec3 position;         // Camera position in world space
    Vec3 forward;          // Forward direction vector (normalized)
    float yaw;             // Horizontal angle in degrees (rotation around Y axis)
    float pitch;           // Vertical angle in degrees (rotation around X axis)
    float cameraSpeed;     // Movement speed (units per second)
    float mouseSensitivity;// Mouse sensitivity for looking around (degrees per pixel)
	
    float m_fovDegrees = 90.0f; // Field of view in degrees
    float m_zNear = 0.1f;       // Near clipping plane
    float m_zFar = 1000.0f;     // Far clipping plane
};
