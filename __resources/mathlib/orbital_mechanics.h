#ifndef ORBITAL_MECHANICS_H
#define ORBITAL_MECHANICS_H

#include "vector3d.h"
#include <cmath>

const double G = 6.67430e-11; // Gravitational constant

// Simple orbital body struct
struct OrbitalBody
{
    Vector3d position;
    Vector3d velocity;
    double mass;

    OrbitalBody() : position(), velocity(), mass(1.0) {}
};

// Update orbital body position and velocity using simple Newtonian gravity (two-body problem)
inline void UpdateOrbit(OrbitalBody& body, const OrbitalBody& centralBody, double deltaTime)
{
    Vector3d direction = centralBody.position - body.position;
    double distance = direction.Length();
    if (distance == 0.0) return;
    Vector3d directionNorm = direction * (1.0 / distance);
    
    // gravitational acceleration magnitude
    double accelMag = G * centralBody.mass / (distance * distance);

    // acceleration vector
    Vector3d acceleration = directionNorm * accelMag;

    // update velocity and position
    body.velocity = body.velocity + acceleration * deltaTime;
    body.position = body.position + body.velocity * deltaTime;
}

#endif // ORBITAL_MECHANICS_H
