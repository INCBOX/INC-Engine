#include "mathlib/matrix4x4_d.h"
#include <cmath>

Matrix4x4_d Matrix4x4_d::Identity() {
    Matrix4x4_d result;
    for (int i = 0; i < 4; i++)
        result[i][i] = 1.0;
    return result;
}

Matrix4x4_d Matrix4x4_d::Translation(const Vector3_d& t) {
    Matrix4x4_d result = Identity();
    result[3][0] = t.x;
    result[3][1] = t.y;
    result[3][2] = t.z;
    return result;
}

Matrix4x4_d Matrix4x4_d::LookAt(const Vector3_d& eye, const Vector3_d& center, const Vector3_d& up) {
    Vector3_d f = (center - eye).Normalize(); // forward
    Vector3_d s = f.Cross(up).Normalize();    // right
    Vector3_d u = s.Cross(f);                  // up

    Matrix4x4_d result = Identity();

    // column-major order: m[col][row]
    result[0][0] = s.x;
    result[0][1] = u.x;
    result[0][2] = -f.x;

    result[1][0] = s.y;
    result[1][1] = u.y;
    result[1][2] = -f.y;

    result[2][0] = s.z;
    result[2][1] = u.z;
    result[2][2] = -f.z;

    result[3][0] = -s.Dot(eye);
    result[3][1] = -u.Dot(eye);
    result[3][2] = f.Dot(eye);

    return result;
}

Matrix4x4_d Matrix4x4_d::Perspective(double fovYDegrees, double aspect, double nearZ, double farZ) {
    double fovRad = fovYDegrees * 3.14159265358979323846 / 180.0;
    double f = 1.0 / std::tan(fovRad / 2.0);

    Matrix4x4_d result;

    result[0][0] = f / aspect;
    result[1][1] = f;
    result[2][2] = (farZ + nearZ) / (nearZ - farZ);
    result[2][3] = -1.0;
    result[3][2] = (2.0 * farZ * nearZ) / (nearZ - farZ);
    result[3][3] = 0.0;

    return result;
}

Matrix4x4_d Matrix4x4_d::Orthographic(double left, double right, double bottom, double top, double nearZ, double farZ) {
    Matrix4x4_d result;

    result[0][0] = 2.0 / (right - left);
    result[1][1] = 2.0 / (top - bottom);
    result[2][2] = -2.0 / (farZ - nearZ);

    result[3][0] = -(right + left) / (right - left);
    result[3][1] = -(top + bottom) / (top - bottom);
    result[3][2] = -(farZ + nearZ) / (farZ - nearZ);
    result[3][3] = 1.0;

    return result;
}

Matrix4x4_d operator*(const Matrix4x4_d& a, const Matrix4x4_d& b) {
    Matrix4x4_d result;
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            double sum = 0.0;
            for (int i = 0; i < 4; ++i) {
                sum += a[i][row] * b[col][i];
            }
            result[col][row] = sum;
        }
    }
    return result;
}

Matrix4x4_d Matrix4x4_d::Translate(const Vector3_d& offset) {
    Matrix4x4_d mat = Matrix4x4_d::Identity();
    mat[3][0] = offset.x;
    mat[3][1] = offset.y;
    mat[3][2] = offset.z;
    return mat;
}
