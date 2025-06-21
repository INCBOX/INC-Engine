#pragma once
#include "mathlib/vector3_f.h"

struct Matrix4x4_f {
    float m[4][4]; // COLUMN-MAJOR: m[col][row]

    float* operator[](int col) { return m[col]; }
    const float* operator[](int col) const { return m[col]; }

    const float* Data() const { return &m[0][0]; }

    static Matrix4x4_f Identity();
    static Matrix4x4_f Translation(const Vector3_f& t);
    static Matrix4x4_f LookAt(const Vector3_f& eye, const Vector3_f& center, const Vector3_f& up);
    static Matrix4x4_f Perspective(float fovYDeg, float aspect, float nearZ, float farZ);

	// FOR SHADOWS
    static Matrix4x4_f Orthographic(float left, float right, float bottom, float top, float nearZ, float farZ);
};

inline Matrix4x4_f operator*(const Matrix4x4_f& a, const Matrix4x4_f& b)
{
    Matrix4x4_f result = {};
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row)
            for (int i = 0; i < 4; ++i)
                result[col][row] += a[i][row] * b[col][i];
    return result;
}

using Mat4_f = Matrix4x4_f;	// Matrix4x4_f and Mat4_f be used interchangeably.