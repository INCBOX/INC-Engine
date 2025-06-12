#include "vmatrix.h"
#include <cmath>

void VMatrix::Identity() {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            m[i][j] = (i == j) ? 1.0f : 0.0f;
}

void VMatrix::Perspective(float fovY, float aspect, float zNear, float zFar) {
    float tanHalfFovy = std::tan(fovY / 2.0f);
    Identity();
    m[0][0] = 1.0f / (aspect * tanHalfFovy);
    m[1][1] = 1.0f / tanHalfFovy;
    m[2][2] = -(zFar + zNear) / (zFar - zNear);
    m[2][3] = -1.0f;
    m[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
    m[3][3] = 0.0f;
}

void VMatrix::LookAt(const Vector& eye, const Vector& target, const Vector& up) {
    Vector f = (target - eye).Normalize();
    Vector s = f.Cross(up).Normalize();
    Vector u = s.Cross(f);

    Identity();
    m[0][0] = s.x; m[1][0] = s.y; m[2][0] = s.z;
    m[0][1] = u.x; m[1][1] = u.y; m[2][1] = u.z;
    m[0][2] = -f.x; m[1][2] = -f.y; m[2][2] = -f.z;

    m[3][0] = -s.Dot(eye);
    m[3][1] = -u.Dot(eye);
    m[3][2] = f.Dot(eye);
}