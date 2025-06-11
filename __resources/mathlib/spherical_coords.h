#ifndef SPHERICAL_COORDS_H
#define SPHERICAL_COORDS_H

#include "vector3d.h"
#include <cmath>

// Convert spherical coords (radius, theta, phi) to Cartesian (x,y,z)
// theta = azimuth angle in radians [0, 2pi]
// phi = polar angle in radians [0, pi]
inline Vector3d SphericalToCartesian(double radius, double theta, double phi)
{
    double sinPhi = std::sin(phi);
    double x = radius * sinPhi * std::cos(theta);
    double y = radius * sinPhi * std::sin(theta);
    double z = radius * std::cos(phi);
    return Vector3d(x, y, z);
}

// Convert Cartesian coords to spherical coords (radius, theta, phi)
inline void CartesianToSpherical(const Vector3d& pos, double& radius, double& theta, double& phi)
{
    radius = pos.Length();
    if (radius == 0.0) {
        theta = 0.0;
        phi = 0.0;
        return;
    }
    theta = std::atan2(pos.y, pos.x);
    if (theta < 0.0) theta += 2.0 * M_PI;
    phi = std::acos(pos.z / radius);
}

#endif // SPHERICAL_COORDS_H
