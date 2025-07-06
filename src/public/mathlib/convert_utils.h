// convert_utils.h
#pragma once
#include "mathlib/vector3_f.h"
#include "mathlib/vector3_d.h"
#include "mathlib/matrix4x4_f.h"
#include "mathlib/matrix4x4_d.h"

// Vector conversions
inline Vector3_f ToFloat(const Vector3_d& v) {
    return Vector3_f((float)v.x, (float)v.y, (float)v.z);
}

inline Vector3_d ToDouble(const Vector3_f& v) {
    return Vector3_d((double)v.x, (double)v.y, (double)v.z);
}

// Matrix conversions
inline Matrix4x4_f ToFloat(const Matrix4x4_d& m) {
    Matrix4x4_f result;
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row)
            result[col][row] = (float)m[col][row];
    return result;
}

inline Matrix4x4_d ToDouble(const Matrix4x4_f& m) {
    Matrix4x4_d result;
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row)
            result[col][row] = (double)m[col][row];
    return result;
}
