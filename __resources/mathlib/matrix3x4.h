#ifndef MATRIX3X4_H
#define MATRIX3X4_H

#include "vector.h"

class matrix3x4_t
{
public:
    float m[3][4];

    matrix3x4_t() {}

    void InitIdentity() {
        m[0][0] = 1.f; m[0][1] = 0.f; m[0][2] = 0.f; m[0][3] = 0.f;
        m[1][0] = 0.f; m[1][1] = 1.f; m[1][2] = 0.f; m[1][3] = 0.f;
        m[2][0] = 0.f; m[2][1] = 0.f; m[2][2] = 1.f; m[2][3] = 0.f;
    }

    Vector GetTranslation() const {
        return Vector(m[0][3], m[1][3], m[2][3]);
    }
};

#endif // MATRIX3X4_H
