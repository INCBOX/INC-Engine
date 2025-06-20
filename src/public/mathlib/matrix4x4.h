// Wire up MVP logic

#pragma once
#include "mathlib/vector.h"

struct Matrix4x4 {
    float m[4][4]; // COLUMN-MAJOR: m[col][row]

    float* operator[](int col) { return m[col]; }
    const float* operator[](int col) const { return m[col]; }

    const float* Data() const { return &m[0][0]; }

    static Matrix4x4 Identity();
    static Matrix4x4 Translation(const Vector& t);
    static Matrix4x4 LookAt(const Vector& eye, const Vector& center, const Vector& up);
    static Matrix4x4 Perspective(float fovYDeg, float aspect, float nearZ, float farZ);

    // FOR SHADOWS
    static Matrix4x4 Orthographic(float left, float right, float bottom, float top, float nearZ, float farZ);
};

inline Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b)
{
    Matrix4x4 result = {};
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row)
            for (int i = 0; i < 4; ++i)
                result[col][row] += a[i][row] * b[col][i];
    return result;
}

using Mat4 = Matrix4x4;  // Matrix and Mat4 be used interchangeably.