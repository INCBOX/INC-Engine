#pragma once

class Vector {
public:
    float x, y, z;

    Vector() : x(0), y(0), z(0) {}
    Vector(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

    Vector operator+(const Vector& v) const { return Vector(x + v.x, y + v.y, z + v.z); }
    Vector operator-(const Vector& v) const { return Vector(x - v.x, y - v.y, z - v.z); }
    Vector operator*(float f) const { return Vector(x * f, y * f, z * f); }
	Vector& operator+=(const Vector& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}
	
	Vector& operator-=(const Vector& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

    float* Base() { return &x; }
    const float* Base() const { return &x; }
	float Length() const;
	float Dot(const Vector& other) const;
	Vector Cross(const Vector& other) const;
	Vector Normalize() const;
};

using Vec3 = Vector;		// Vector and Vec3 be used interchangeably.