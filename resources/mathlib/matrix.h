#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

class matrix4x4_t
{
public:
    float m[4][4];

    matrix4x4_t() {}

    void InitIdentity() {
        for (int i=0; i<4; i++)
            for (int j=0; j<4; j++)
                m[i][j] = (i == j) ? 1.f : 0.f;
    }
};

#endif // MATRIX_H
