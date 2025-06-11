#pragma once
#include <cmath>

namespace mathlib {

struct Vec4 {
    float x, y, z, w;

    Vec4() : x(0), y(0), z(0), w(0) {}
    Vec4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}

    Vec4 operator+(const Vec4& o) const { return Vec4(x + o.x, y + o.y, z + o.z, w + o.w); }
    Vec4 operator-(const Vec4& o) const { return Vec4(x - o.x, y - o.y, z - o.z, w - o.w); }
    Vec4 operator*(float s) const { return Vec4(x * s, y * s, z * s, w * s); }
    Vec4 operator/(float s) const { return Vec4(x / s, y / s, z / s, w / s); }

    Vec4& operator+=(const Vec4& o) { x += o.x; y += o.y; z += o.z; w += o.w; return *this; }
    Vec4& operator-=(const Vec4& o) { x -= o.x; y -= o.y; z -= o.z; w -= o.w; return *this; }
    Vec4& operator*=(float s) { x *= s; y *= s; z *= s; w *= s; return *this; }
    Vec4& operator/=(float s) { x /= s; y /= s; z /= s; w /= s; return *this; }

    float length() const { return std::sqrt(x*x + y*y + z*z + w*w); }
    float lengthSq() const { return x*x + y*y + z*z + w*w; }

    Vec4 normalized() const {
        float len = length();
        if (len == 0) return Vec4(0,0,0,0);
        return Vec4(x / len, y / len, z / len, w / len);
    }

    float dot(const Vec4& o) const { return x*o.x + y*o.y + z*o.z + w*o.w; }
};

} // namespace mathlib
