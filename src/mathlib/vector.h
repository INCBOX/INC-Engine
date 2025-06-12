#pragma once

class Vector {
public:
    float x, y, z;

    Vector() : x(0), y(0), z(0) {}
    Vector(float fx, float fy, float fz) : x(fx), y(fy), z(fz) {}

    float Length() const;
    float LengthSqr() const;
    Vector Normalize() const;
    float Dot(const Vector& v) const;
    Vector Cross(const Vector& v) const;

    Vector operator+(const Vector& v) const;
    Vector operator-(const Vector& v) const;
    Vector operator*(float scalar) const;
    Vector& operator+=(const Vector& v);
    Vector& operator-=(const Vector& v);
};