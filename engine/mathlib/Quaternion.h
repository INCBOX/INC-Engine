#pragma once
#include <cmath>
#include "Vec3.h"

namespace mathlib {

struct Quaternion {
    float w, x, y, z;

    Quaternion() : w(1), x(0), y(0), z(0) {}
    Quaternion(float w_, float x_, float y_, float z_) : w(w_), x(x_), y(y_), z(z_) {}

    static Quaternion Identity() {
        return Quaternion(1,0,0,0);
    }

    static Quaternion FromAxisAngle(const Vec3& axis, float angleRad) {
        float halfAngle = angleRad * 0.5f;
        float s = std::sin(halfAngle);
        return Quaternion(std::cos(halfAngle), axis.x * s, axis.y * s, axis.z * s).Normalized();
    }

    Quaternion Normalized() const {
        float len = std::sqrt(w*w + x*x + y*y + z*z);
        return Quaternion(w/len, x/len, y/len, z/len);
    }

    Quaternion Conjugate() const {
        return Quaternion(w, -x, -y, -z);
    }

    Quaternion operator*(const Quaternion& rhs) const {
        return Quaternion(
            w*rhs.w - x*rhs.x - y*rhs.y - z*rhs.z,
            w*rhs.x + x*rhs.w + y*rhs.z - z*rhs.y,
            w*rhs.y - x*rhs.z + y*rhs.w + z*rhs.x,
            w*rhs.z + x*rhs.y - y*rhs.x + z*rhs.w
        );
    }

    Vec3 Rotate(const Vec3& v) const {
        Quaternion qv(0, v.x, v.y, v.z);
        Quaternion result = (*this) * qv * Conjugate();
        return Vec3(result.x, result.y, result.z);
    }

    static Quaternion FromEulerAngles(float pitch, float yaw, float roll) {
        // Assuming order: yaw (Y), pitch (X), roll (Z)
        float cy = std::cos(yaw * 0.5f);
        float sy = std::sin(yaw * 0.5f);
        float cp = std::cos(pitch * 0.5f);
        float sp = std::sin(pitch * 0.5f);
        float cr = std::cos(roll * 0.5f);
        float sr = std::sin(roll * 0.5f);

        return Quaternion(
            cr * cp * cy + sr * sp * sy,
            sr * cp * cy - cr * sp * sy,
            cr * sp * cy + sr * cp * sy,
            cr * cp * sy - sr * sp * cy
        );
    }
};

} // namespace mathlib
