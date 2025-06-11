#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <cmath>

class Vector3d
{
public:
    double x, y, z;

    Vector3d() : x(0), y(0), z(0) {}
    Vector3d(double X, double Y, double Z) : x(X), y(Y), z(Z) {}

    double Length() const {
        return std::sqrt(x*x + y*y + z*z);
    }

    Vector3d operator+(const Vector3d& v) const {
        return Vector3d(x+v.x, y+v.y, z+v.z);
    }

    Vector3d operator-(const Vector3d& v) const {
        return Vector3d(x-v.x, y-v.y, z-v.z);
    }

    Vector3d operator*(double f) const {
        return Vector3d(x*f, y*f, z*f);
    }

    double Dot(const Vector3d& v) const {
        return x*v.x + y*v.y + z*v.z;
    }

    Vector3d Cross(const Vector3d& v) const {
        return Vector3d(
            y*v.z - z*v.y,
            z*v.x - x*v.z,
            x*v.y - y*v.x
        );
    }
};

#endif // VECTOR3D_H
