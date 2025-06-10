
#ifndef INC_ENGINE_MATH_H
#define INC_ENGINE_MATH_H

#include <iostream>

#include <cmath>
#include <cstring>

struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}

    Vec3 operator+(const Vec3& o) const { return Vec3(x + o.x, y + o.y, z + o.z); }
    Vec3 operator-(const Vec3& o) const { return Vec3(x - o.x, y - o.y, z - o.z); }
    Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
    Vec3 operator/(float s) const { return Vec3(x / s, y / s, z / s); }

    Vec3& operator+=(const Vec3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vec3& operator-=(const Vec3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }

    float dot(const Vec3& o) const { return x * o.x + y * o.y + z * o.z; }
    Vec3 cross(const Vec3& o) const {
        return Vec3(
            y * o.z - z * o.y,
            z * o.x - x * o.z,
            x * o.y - y * o.x
        );
    }

    float length() const { return std::sqrt(x*x + y*y + z*z); }
    Vec3 normalized() const {
        float len = length();
        return len > 0 ? *this / len : Vec3();
    }
};

struct Mat4 {
    float m[16];

    const float* toGLMatrix() const {
        return &m[0];
    }

    Mat4() {
        for (int i = 0; i < 16; ++i) m[i] = 0;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }

    static Mat4 identity() {
        return Mat4();
    }

    static Mat4 perspective(float fovRadians, float aspect, float nearPlane, float farPlane) {
        Mat4 result;
        float f = 1.0f / std::tan(fovRadians / 2.0f);
        result.m[0] = f / aspect;
        result.m[5] = f;
        result.m[10] = (farPlane + nearPlane) / (nearPlane - farPlane);
        result.m[11] = -1.0f;
        result.m[14] = (2 * farPlane * nearPlane) / (nearPlane - farPlane);
        result.m[15] = 0.0f;
        return result;
    }

    static Mat4 translate(const Vec3& v) {
        Mat4 result = identity();
        result.m[12] = v.x;
        result.m[13] = v.y;
        result.m[14] = v.z;
        return result;
    }

    static Mat4 scale(float s) {
        Mat4 result = identity();
        result.m[0] = s;
        result.m[5] = s;
        result.m[10] = s;
        return result;
    }
	
    static Mat4 rotateY(float angleRadians) {
        Mat4 result = identity();
        float c = std::cos(angleRadians);
        float s = std::sin(angleRadians);
        result.m[0] = c;
        result.m[2] = -s;
        result.m[8] = s;
        result.m[10] = c;
        return result;
	}
	
    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = (center - eye).normalized();
        Vec3 s = f.cross(up).normalized();
        Vec3 u = s.cross(f);

        Mat4 result = identity();
        result.m[0] = s.x;
        result.m[4] = s.y;
        result.m[8] = s.z;

        result.m[1] = u.x;
        result.m[5] = u.y;
        result.m[9] = u.z;

        result.m[2] = -f.x;
        result.m[6] = -f.y;
        result.m[10] = -f.z;

        result.m[12] = -s.dot(eye);
        result.m[13] = -u.dot(eye);
        result.m[14] = f.dot(eye);
        return result;
    }

    static Mat4 orthographic(float left, float right, float bottom, float top, float nearZ, float farZ) {
        Mat4 result;
        result.m[0] = 2.0f / (right - left);
        result.m[5] = 2.0f / (top - bottom);
        result.m[10] = -2.0f / (farZ - nearZ);
        result.m[12] = -(right + left) / (right - left);
        result.m[13] = -(top + bottom) / (top - bottom);
        result.m[14] = -(farZ + nearZ) / (farZ - nearZ);
        result.m[15] = 1.0f;
        return result;
    }

    static Mat4 fromArray(const float* arr) {
        Mat4 result;
        std::memcpy(result.m, arr, sizeof(float) * 16);
        return result;
    }
	
	static Mat4 scale(const Vec3& v) {
		Mat4 result = identity();
		result.m[0] = v.x;
		result.m[5] = v.y;
		result.m[10] = v.z;
		return result;
	}
	
	
};

inline Mat4 operator*(const Mat4& a, const Mat4& b) {
    Mat4 result;
    for (int row = 0; row < 4; ++row)
        for (int col = 0; col < 4; ++col)
            result.m[col + row * 4] =
                a.m[0 + row * 4] * b.m[col + 0] +
                a.m[1 + row * 4] * b.m[col + 4] +
                a.m[2 + row * 4] * b.m[col + 8] +
                a.m[3 + row * 4] * b.m[col +12];
    return result;
}

inline float radians(float degrees) {
    return degrees * 3.14159265359f / 180.0f;
}

// ---- Modular MVP Support ----
extern Mat4 gViewMatrix;
extern Mat4 gProjMatrix;

inline Mat4 MakeMVP(const Mat4& model) {
    return gProjMatrix * gViewMatrix * model;
}

inline void PrintMVP(const Mat4& mvp) {
    std::cout << "[MVP] ";
    for (int i = 0; i < 16; ++i)
        std::cout << mvp.m[i] << ' ';
    std::cout << std::endl;
}

#endif // INC_ENGINE_MATH_H
