// Wire up MVP logic

#pragma once

#include "vec3.h"

struct Mat4 {
    float m[4][4];

    float* operator[](int row) { return m[row]; }
    const float* operator[](int row) const { return m[row]; }

    static Mat4 Identity();
    static Mat4 Translation(const Vec3& t);
    static Mat4 LookAt(const Vec3& eye, const Vec3& center, const Vec3& up);
    static Mat4 Perspective(float fovYDeg, float aspect, float nearZ, float farZ);

    const float* Data() const { return &m[0][0]; }
};

inline Mat4 operator*(const Mat4& a, const Mat4& b) {
    Mat4 result = {};
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            for (int k = 0; k < 4; ++k)
                result.m[i][j] += a.m[i][k] * b.m[k][j];
    return result;
}
