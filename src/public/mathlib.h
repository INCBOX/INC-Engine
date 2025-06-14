#ifndef MATHLIB_H
#define MATHLIB_H

#include <cmath>
#include <cassert>

using vec_t = float;

struct Vector {
    vec_t x, y, z;

    Vector() : x(0), y(0), z(0) {}
    Vector(vec_t X, vec_t Y, vec_t Z) : x(X), y(Y), z(Z) {}

    // Access operators
    vec_t& operator[](int i) {
        assert(i >= 0 && i < 3);
        return *((&x) + i);
    }
    const vec_t& operator[](int i) const {
        assert(i >= 0 && i < 3);
        return *((&x) + i);
    }

    // Arithmetic operators
    Vector operator+(const Vector& v) const { return Vector(x + v.x, y + v.y, z + v.z); }
    Vector operator-(const Vector& v) const { return Vector(x - v.x, y - v.y, z - v.z); }
    Vector operator*(vec_t fl) const { return Vector(x * fl, y * fl, z * fl); }
    Vector operator/(vec_t fl) const { assert(fl != 0); return Vector(x / fl, y / fl, z / fl); }

    // Dot product
    vec_t Dot(const Vector& v) const { return x * v.x + y * v.y + z * v.z; }

    // Cross product
    Vector Cross(const Vector& v) const {
        return Vector(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

    // Length and normalization
    vec_t Length() const { return std::sqrt(x*x + y*y + z*z); }
    vec_t LengthSqr() const { return x*x + y*y + z*z; }

    void NormalizeInPlace() {
        vec_t len = Length();
        if (len != 0) {
            x /= len; y /= len; z /= len;
        }
    }

    Vector Normalized() const {
        vec_t len = Length();
        if (len == 0) return Vector(0,0,0);
        return Vector(x / len, y / len, z / len);
    }
};

// 3x4 matrix for affine transforms (rotation + translation)
struct matrix3x4_t {
    float m[3][4];

    // Default constructor = identity matrix
    matrix3x4_t() {
        m[0][0] = 1.f; m[0][1] = 0.f; m[0][2] = 0.f; m[0][3] = 0.f;
        m[1][0] = 0.f; m[1][1] = 1.f; m[1][2] = 0.f; m[1][3] = 0.f;
        m[2][0] = 0.f; m[2][1] = 0.f; m[2][2] = 1.f; m[2][3] = 0.f;
    }

    // Set translation component
    void SetOrigin(const Vector& origin) {
        m[0][3] = origin.x;
        m[1][3] = origin.y;
        m[2][3] = origin.z;
    }

    // Get translation component
    Vector GetOrigin() const {
        return Vector(m[0][3], m[1][3], m[2][3]);
    }
};

// Multiply a vector by matrix3x4_t (transform vector by rotation + translation)
inline Vector VectorTransform(const Vector& v, const matrix3x4_t& m) {
    return Vector(
        v.x * m.m[0][0] + v.y * m.m[0][1] + v.z * m.m[0][2] + m.m[0][3],
        v.x * m.m[1][0] + v.y * m.m[1][1] + v.z * m.m[1][2] + m.m[1][3],
        v.x * m.m[2][0] + v.y * m.m[2][1] + v.z * m.m[2][2] + m.m[2][3]
    );
}

#endif // MATHLIB_H