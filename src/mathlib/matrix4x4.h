#pragma once
#include "vector.h"
#include <cstring>

class matrix4x4_t {
public:
    float m[4][4];

    matrix4x4_t() {
        std::memset(m, 0, sizeof(m));
    }

    void InitIdentity() {
        std::memset(m, 0, sizeof(m));
        m[0][0] = 1.0f;
        m[1][1] = 1.0f;
        m[2][2] = 1.0f;
        m[3][3] = 1.0f;
    }

    static matrix4x4_t BuildTranslation(const Vector& v) {
        matrix4x4_t mat;
        mat.InitIdentity();
        mat.m[0][3] = v.x;
        mat.m[1][3] = v.y;
        mat.m[2][3] = v.z;
        return mat;
    }

    const float* Base() const { return &m[0][0]; }
};
