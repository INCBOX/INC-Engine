#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <emmintrin.h> // SSE2

class Vector
{
public:
    float x, y, z;

    Vector() : x(0), y(0), z(0) {}
    Vector(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

    inline float Length() const {
        return sqrtf(x*x + y*y + z*z);
    }

    inline Vector operator+(const Vector& v) const {
        return Vector(x+v.x, y+v.y, z+v.z);
    }

    inline Vector operator-(const Vector& v) const {
        return Vector(x-v.x, y-v.y, z-v.z);
    }

    inline Vector operator*(float f) const {
        return Vector(x*f, y*f, z*f);
    }

    inline float Dot(const Vector& v) const {
        return x*v.x + y*v.y + z*v.z;
    }

    inline Vector Cross(const Vector& v) const {
        return Vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    }
};

#endif // VECTOR_H