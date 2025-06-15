#pragma once

class Vector {
public:
    float x, y, z;

    Vector() : x(0), y(0), z(0) {}
    Vector(float X, float Y, float Z) : x(X), y(Y), z(Z) {}

    Vector operator+(const Vector& v) const { return Vector(x + v.x, y + v.y, z + v.z); }
    Vector operator-(const Vector& v) const { return Vector(x - v.x, y - v.y, z - v.z); }
    Vector operator*(float f) const { return Vector(x * f, y * f, z * f); }

    float* Base() { return &x; }
    const float* Base() const { return &x; }
};
