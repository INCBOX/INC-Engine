#pragma once

class Vector3_f {
public:
    float x, y, z;

    Vector3_f() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3_f(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

    Vector3_f operator+(const Vector3_f& v) const { return Vector3_f(x + v.x, y + v.y, z + v.z); }
    Vector3_f operator-(const Vector3_f& v) const { return Vector3_f(x - v.x, y - v.y, z - v.z); }
    Vector3_f operator*(float f) const { return Vector3_f(x * f, y * f, z * f); }
    
    Vector3_f& operator+=(const Vector3_f& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    
    Vector3_f& operator-=(const Vector3_f& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
	
    // unary minus operator:
    Vector3_f operator-() const {
        return Vector3_f(-x, -y, -z);
    }

    Vector3_f& operator*=(float f); // physics math used for movement physics
    Vector3_f& operator/=(float f); // physics math
    Vector3_f ProjectOnto(const Vector3_f& other) const; // (used for friction, sliding, etc)

    float* Base() { return &x; }
    const float* Base() const { return &x; }
    float Length() const;
    float LengthSqr() const;
    float Dot(const Vector3_f& other) const;
    Vector3_f Cross(const Vector3_f& other) const;
    Vector3_f Normalize() const;
    
    bool IsZero(float epsilon = 1e-6f) const; // movement physics stuff
};

using Vec3_f = Vector3_f; // Alias for float precision vector3