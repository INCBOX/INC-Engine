// Wire up MVP logic

#pragma once

#include "mathlib/vector.h"

struct Matrix {
    float m[4][4];

    float* operator[](int row) { return m[row]; }
    const float* operator[](int row) const { return m[row]; }

    static Matrix Identity();
    static Matrix Translation(const Vector& t);
    static Matrix LookAt(const Vector& eye, const Vector& center, const Vector& up);
    static Matrix Perspective(float fovYDeg, float aspect, float nearZ, float farZ);

    const float* Data() const { return &m[0][0]; }
};

inline Matrix operator*(const Matrix& a, const Matrix& b) {
    Matrix result = {};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            for (int k = 0; k < 4; ++k)
                result.m[i][j] += a.m[i][k] * b.m[k][j];
    return result;
}
