#pragma once
#include "vector.h"
#include <cstring>

class matrix3x4_t {
public:
    float m[3][4];

    matrix3x4_t() {
        std::memset(m, 0, sizeof(m));
    }

    void InitIdentity() {
        std::memset(m, 0, sizeof(m));
        m[0][0] = 1.0f;
        m[1][1] = 1.0f;
        m[2][2] = 1.0f;
    }

    void SetOrigin(const Vector& origin) {
        m[0][3] = origin.x;
        m[1][3] = origin.y;
        m[2][3] = origin.z;
    }

    // Returns pointer to raw data for GL
    const float* Base() const { return &m[0][0]; }
};
