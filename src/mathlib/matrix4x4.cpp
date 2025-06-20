#include <cmath>
#include <iostream> // REMOVE LATER required for std::cout
#include "mathlib/matrix4x4.h"

Matrix4x4 Matrix4x4::Identity() {
    Matrix4x4 result = {};
    for (int i = 0; i < 4; i++)
        result[i][i] = 1.0f;
    return result;
}

Matrix4x4 Matrix4x4::Translation(const Vector& t) {
    Matrix4x4 result = Matrix4x4::Identity();
    result[3][0] = t.x;
    result[3][1] = t.y;
    result[3][2] = t.z;
    return result;
}

Matrix4x4 Matrix4x4::LookAt(const Vector& eye, const Vector& center, const Vector& up)
{
    Vector f = (center - eye).Normalize(); // forward
    Vector s = f.Cross(up).Normalize();    // right
    Vector u = s.Cross(f);                 // up

    Matrix4x4 result = Matrix4x4::Identity();

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
    result[3][2] = f.Dot(eye); // right-handed OpenGL

    return result;
}

Matrix4x4 Matrix4x4::Perspective(float fovYDegrees, float aspect, float nearZ, float farZ) {
    float fovRad = fovYDegrees * 3.14159265f / 180.0f;
    // std::cout << "[Matrix4x4::Perspective] FOV radians: " << fovRad << ", aspect: " << aspect << "\n";
    float f = 1.0f / tan(fovRad / 2.0f);

    Matrix4x4 result = {};

    result[0][0] = f / aspect;
    result[1][1] = f;
    result[2][2] = (farZ + nearZ) / (nearZ - farZ);
    result[2][3] = -1.0f;
    result[3][2] = (2.0f * farZ * nearZ) / (nearZ - farZ);
    return result;
}

// FOR SHADOWS [NOT USED YET]
Matrix4x4 Matrix4x4::Orthographic(float left, float right, float bottom, float top, float nearZ, float farZ) {
    Matrix4x4 result = {};

    result[0][0] = 2.0f / (right - left);
    result[1][1] = 2.0f / (top - bottom);
    result[2][2] = -2.0f / (farZ - nearZ);

    result[3][0] = -(right + left) / (right - left);
    result[3][1] = -(top + bottom) / (top - bottom);
    result[3][2] = -(farZ + nearZ) / (farZ - nearZ);
    result[3][3] = 1.0f;

    return result;
}