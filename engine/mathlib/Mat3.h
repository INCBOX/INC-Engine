#pragma once
#include <cmath>
#include "Vec3.h"

namespace mathlib {

struct Mat3 {
    float m[3][3];

    Mat3() {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }

    Mat3(float diagonal) {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                m[i][j] = (i == j) ? diagonal : 0.0f;
    }

    static Mat3 Identity() {
        return Mat3(1.0f);
    }

    Mat3 operator*(const Mat3& o) const {
        Mat3 result(0.0f);
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                for (int k = 0; k < 3; k++)
                    result.m[i][j] += m[i][k] * o.m[k][j];
        return result;
    }

    Vec3 operator*(const Vec3& v) const {
        return Vec3(
            m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z,
            m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z,
            m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z
        );
    }

    static Mat3 RotationX(float angleRad) {
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        Mat3 r;
        r.m[1][1] = c; r.m[1][2] = -s;
        r.m[2][1] = s; r.m[2][2] = c;
        return r;
    }

    static Mat3 RotationY(float angleRad) {
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        Mat3 r;
        r.m[0][0] = c; r.m[0][2] = s;
        r.m[2][0] = -s; r.m[2][2] = c;
        return r;
    }

    static Mat3 RotationZ(float angleRad) {
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        Mat3 r;
        r.m[0][0] = c; r.m[0][1] = -s;
        r.m[1][0] = s; r.m[1][1] = c;
        return r;
    }
};

} // namespace mathlib
