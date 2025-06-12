#include <cmath>

static Mat4 LookAt(const Vec3& eye, const Vec3& center, const Vec3& up);
static Mat4 Perspective(float fovYDeg, float aspect, float nearZ, float farZ);

Mat4 Mat4::LookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
    Vec3 f = (center - eye).Normalize();
    Vec3 s = f.Cross(up).Normalize();
    Vec3 u = s.Cross(f);

    Mat4 result = Mat4::Identity();
    result[0][0] = s.x;
    result[1][0] = s.y;
    result[2][0] = s.z;

    result[0][1] = u.x;
    result[1][1] = u.y;
    result[2][1] = u.z;

    result[0][2] = -f.x;
    result[1][2] = -f.y;
    result[2][2] = -f.z;

    result[3][0] = -s.Dot(eye);
    result[3][1] = -u.Dot(eye);
    result[3][2] = f.Dot(eye);
    return result;
}

Mat4 Mat4::Perspective(float fovYDegrees, float aspect, float nearZ, float farZ) {
    float fovRad = fovYDegrees * 3.14159265f / 180.0f;
    float f = 1.0f / tan(fovRad / 2.0f);

    Mat4 result = {};
    result[0][0] = f / aspect;
    result[1][1] = f;
    result[2][2] = (farZ + nearZ) / (nearZ - farZ);
    result[2][3] = -1.0f;
    result[3][2] = (2 * farZ * nearZ) / (nearZ - farZ);
    return result;
}