#pragma once
#include "mathlib/vector3_d.h"

struct Matrix4x4_d {
    double m[4][4]; // COLUMN-MAJOR: m[col][row]

    double* operator[](int col) { return m[col]; }
    const double* operator[](int col) const { return m[col]; }

    const double* Data() const { return &m[0][0]; }

    static Matrix4x4_d Identity();
    static Matrix4x4_d Translation(const Vector3_d& t);
    static Matrix4x4_d LookAt(const Vector3_d& eye, const Vector3_d& center, const Vector3_d& up);
    static Matrix4x4_d Perspective(double fovYDeg, double aspect, double nearZ, double farZ);

	// FOR SHADOWS
    static Matrix4x4_d Orthographic(double left, double right, double bottom, double top, double nearZ, double farZ);
};

inline Matrix4x4_d operator*(const Matrix4x4_d& a, const Matrix4x4_d& b)
{
    Matrix4x4_d result = {};
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row)
            for (int i = 0; i < 4; ++i)
                result[col][row] += a[i][row] * b[col][i];
    return result;
}

using Mat4_d = Matrix4x4_d;	// Matrix and Mat4 be used interchangeably.