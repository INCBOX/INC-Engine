#pragma once
#include <cmath>

struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

    Vec3 operator+(const Vec3& rhs) const { return Vec3(x + rhs.x, y + rhs.y, z + rhs.z); }
    Vec3 operator-(const Vec3& rhs) const { return Vec3(x - rhs.x, y - rhs.y, z - rhs.z); }
    Vec3 operator*(float scalar)   const { return Vec3(x * scalar, y * scalar, z * scalar); }

    float Dot(const Vec3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }

    Vec3 Cross(const Vec3& rhs) const {
        return Vec3(
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        );
    }

    float Length() const { return std::sqrt(x * x + y * y + z * z); }

    Vec3 Normalize() const {
        float len = Length();
        return len > 0 ? (*this) * (1.0f / len) : Vec3(0, 0, 0);
    }
};
