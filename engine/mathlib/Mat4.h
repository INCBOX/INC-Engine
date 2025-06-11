#pragma once
#include <cmath>
#include "Vec3.h"
#include "Quaternion.h"

namespace mathlib {

struct Mat4 {
    float m[4][4];

    Mat4() {
        for(int i=0; i<4; ++i)
            for(int j=0; j<4; ++j)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }

    static Mat4 Identity() {
        return Mat4();
    }

    static Mat4 Translate(const Vec3& v) { // Alias for Translation
        return Translation(v);
    }

    static Mat4 Translation(const Vec3& v) {
        Mat4 result = Identity();
        result.m[0][3] = v.x;
        result.m[1][3] = v.y;
        result.m[2][3] = v.z;
        return result;
    }

    static Mat4 Scale(const Vec3& v) {
        Mat4 result = Identity();
        result.m[0][0] = v.x;
        result.m[1][1] = v.y;
        result.m[2][2] = v.z;
        return result;
    }

    static Mat4 RotationEuler(float pitch, float yaw, float roll) {
        float cp = std::cos(pitch);
        float sp = std::sin(pitch);
        float cy = std::cos(yaw);
        float sy = std::sin(yaw);
        float cr = std::cos(roll);
        float sr = std::sin(roll);

        Mat4 rx = Identity();
        rx.m[1][1] = cp; rx.m[1][2] = -sp;
        rx.m[2][1] = sp; rx.m[2][2] = cp;

        Mat4 ry = Identity();
        ry.m[0][0] = cy; ry.m[0][2] = sy;
        ry.m[2][0] = -sy; ry.m[2][2] = cy;

        Mat4 rz = Identity();
        rz.m[0][0] = cr; rz.m[0][1] = -sr;
        rz.m[1][0] = sr; rz.m[1][1] = cr;

        return rz * rx * ry;
    }

    static Mat4 RotationQuaternion(const Quaternion& q);

    static Mat4 Perspective(float fovRadians, float aspectRatio, float nearPlane, float farPlane) {
        Mat4 result{};
        float f = 1.0f / std::tan(fovRadians / 2.0f);
        result.m[0][0] = f / aspectRatio;
        result.m[1][1] = f;
        result.m[2][2] = (farPlane + nearPlane) / (nearPlane - farPlane);
        result.m[2][3] = (2 * farPlane * nearPlane) / (nearPlane - farPlane);
        result.m[3][2] = -1.0f;
        result.m[3][3] = 0.0f;
        return result;
    }

    static Mat4 LookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
        Vec3 f = (target - eye).Normalized();
        Vec3 s = f.Cross(up).Normalized();
        Vec3 u = s.Cross(f);

        Mat4 result = Identity();
        result.m[0][0] = s.x; result.m[0][1] = s.y; result.m[0][2] = s.z;
        result.m[1][0] = u.x; result.m[1][1] = u.y; result.m[1][2] = u.z;
        result.m[2][0] = -f.x; result.m[2][1] = -f.y; result.m[2][2] = -f.z;

        result.m[0][3] = -s.Dot(eye);
        result.m[1][3] = -u.Dot(eye);
        result.m[2][3] = f.Dot(eye);
        return result;
    }

	static Mat4 Orthographic(float left, float right, float bottom, float top, float nearZ, float farZ) {
		Mat4 result{};
		result.m[0][0] = 2.0f / (right - left);
		result.m[1][1] = 2.0f / (top - bottom);
		result.m[2][2] = -2.0f / (farZ - nearZ);
		result.m[3][3] = 1.0f;

		result.m[0][3] = -(right + left) / (right - left);
		result.m[1][3] = -(top + bottom) / (top - bottom);
		result.m[2][3] = -(farZ + nearZ) / (farZ - nearZ);

		return result;
	}

    Mat4 operator*(const Mat4& rhs) const {
        Mat4 result;
        for (int row=0; row<4; ++row)
            for (int col=0; col<4; ++col)
                for (int k=0; k<4; ++k)
                    result.m[row][col] += m[row][k] * rhs.m[k][col];
        return result;
    }

    Mat4 Transposed() const {
        Mat4 result;
        for(int i=0; i<4; ++i)
            for(int j=0; j<4; ++j)
                result.m[i][j] = m[j][i];
        return result;
    }

    float* Data() { return &m[0][0]; }
    const float* Data() const { return &m[0][0]; }
};

inline Mat4 Mat4::RotationQuaternion(const Quaternion& q) {
    Mat4 result = Identity();

    float xx = q.x * q.x;
    float yy = q.y * q.y;
    float zz = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    result.m[0][0] = 1.0f - 2.0f * (yy + zz);
    result.m[0][1] = 2.0f * (xy - wz);
    result.m[0][2] = 2.0f * (xz + wy);

    result.m[1][0] = 2.0f * (xy + wz);
    result.m[1][1] = 1.0f - 2.0f * (xx + zz);
    result.m[1][2] = 2.0f * (yz - wx);

    result.m[2][0] = 2.0f * (xz - wy);
    result.m[2][1] = 2.0f * (yz + wx);
    result.m[2][2] = 1.0f - 2.0f * (xx + yy);

    return result;
}

} // namespace mathlib
