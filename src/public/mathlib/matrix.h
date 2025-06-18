// Wire up MVP logic

#pragma once
#include "mathlib/vector.h"

struct Matrix {
    float m[4][4]; // COLUMN-MAJOR: m[col][row]

    float* operator[](int col) { return m[col]; }
    const float* operator[](int col) const { return m[col]; }

    const float* Data() const { return &m[0][0]; }

    static Matrix Identity();
    static Matrix Translation(const Vector& t);
    static Matrix LookAt(const Vector& eye, const Vector& center, const Vector& up);
    static Matrix Perspective(float fovYDeg, float aspect, float nearZ, float farZ);
	
	// FOR SHADOWS
	static Matrix Orthographic(float left, float right, float bottom, float top, float nearZ, float farZ);
};

inline Matrix operator*(const Matrix& a, const Matrix& b)
{
    Matrix result = {};
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row)
            for (int i = 0; i < 4; ++i)
                result[col][row] += a[i][row] * b[col][i];
    return result;
}

using Mat4 = Matrix;		// Matrix and Mat4 be used interchangeably.