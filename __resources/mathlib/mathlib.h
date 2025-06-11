#ifndef MATHLIB_H
#define MATHLIB_H

#include <cmath>

inline float DEG2RAD(float deg) {
    return deg * (3.14159265358979323846f / 180.f);
}

inline float RAD2DEG(float rad) {
    return rad * (180.f / 3.14159265358979323846f);
}

void MathLib_Init() {
    // Init mathlib constants if needed
}

#endif // MATHLIB_H
