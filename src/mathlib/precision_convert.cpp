#include "mathlib/precision_convert.h"

namespace precision {

Vector3_f ToFloat(const Vector3_d& v) {
    return Vector3_f(static_cast<float>(v.x), static_cast<float>(v.y), static_cast<float>(v.z));
}

Vector3_d ToDouble(const Vector3_f& v) {
    return Vector3_d(static_cast<double>(v.x), static_cast<double>(v.y), static_cast<double>(v.z));
}


// Convert double precision matrix to float precision matrix
Matrix4x4_f ToFloat(const Matrix4x4_d& m) {
    Matrix4x4_f out;
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            out.m[col][row] = static_cast<float>(m.m[col][row]);
        }
    }
    return out;
}
// Convert float precision matrix to double precision matrix
Matrix4x4_d ToDouble(const Matrix4x4_f& m) {
    Matrix4x4_d out;
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            out.m[col][row] = static_cast<double>(m.m[col][row]);
        }
    }
    return out;
}

} // namespace precision