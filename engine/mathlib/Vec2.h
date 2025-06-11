#pragma once
#include <cmath>
#include <iostream>

namespace mathlib {

struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float scalar) : x(scalar), y(scalar), z(scalar) {} // ✅ New
    Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    float Length() const {
        return std::sqrt(x*x + y*y + z*z);
    }

    float LengthSq() const { return x*x + y*y + z*z; } // ✅ New

    Vec3 Normalized() const {
        float len = Length();
        if (len == 0) return Vec3(0);
        return Vec3(x/len, y/len, z/len);
    }

    Vec3 operator+(const Vec3& rhs) const { return Vec3(x + rhs.x, y + rhs.y, z + rhs.z); }
    Vec3 operator-(const Vec3& rhs) const { return Vec3(x - rhs.x, y - rhs.y, z - rhs.z); }
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    Vec3 operator/(float scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }

    Vec3& operator+=(const Vec3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
    Vec3& operator-=(const Vec3& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
    Vec3& operator*=(float scalar)    { x *= scalar; y *= scalar; z *= scalar; return *this; }
    Vec3& operator/=(float scalar)    { x /= scalar; y /= scalar; z /= scalar; return *this; }

    float Dot(const Vec3& rhs) const {
        return x*rhs.x + y*rhs.y + z*rhs.z;
    }

    Vec3 Cross(const Vec3& rhs) const {
        return Vec3(
            y*rhs.z - z*rhs.y,
            z*rhs.x - x*rhs.z,
            x*rhs.y - y*rhs.x
        );
    }

    static Vec3 Lerp(const Vec3& a, const Vec3& b, float t) {
        return a + (b - a) * t;
    }

    bool operator==(const Vec3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; } // ✅
    bool operator!=(const Vec3& rhs) const { return !(*this == rhs); } // ✅

    friend Vec3 operator*(float scalar, const Vec3& vec) { return Vec3(vec.x * scalar, vec.y * scalar, vec.z * scalar); }

    friend std::ostream& operator<<(std::ostream& os, const Vec3& v) {
        os << "[" << v.x << ", " << v.y << ", " << v.z << "]";
        return os;
    }
};

} // namespace mathlib
