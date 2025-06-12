#include "vector.h"
#include <cmath>

float Vector::Length() const {
    return std::sqrt(x * x + y * y + z * z);
}

float Vector::LengthSqr() const {
    return x * x + y * y + z * z;
}

Vector Vector::Normalize() const {
    float len = Length();
    return (len > 0) ? Vector(x / len, y / len, z / len) : Vector(0, 0, 0);
}

float Vector::Dot(const Vector& v) const {
    return x * v.x + y * v.y + z * v.z;
}

Vector Vector::Cross(const Vector& v) const {
    return Vector(
        y * v.z - z * v.y,
        z * v.x - x * v.z,
        x * v.y - y * v.x
    );
}

Vector Vector::operator+(const Vector& v) const {
    return Vector(x + v.x, y + v.y, z + v.z);
}

Vector Vector::operator-(const Vector& v) const {
    return Vector(x - v.x, y - v.y, z - v.z);
}

Vector Vector::operator*(float scalar) const {
    return Vector(x * scalar, y * scalar, z * scalar);
}

Vector& Vector::operator+=(const Vector& v) {
    x += v.x; y += v.y; z += v.z;
    return *this;
}

Vector& Vector::operator-=(const Vector& v) {
    x -= v.x; y -= v.y; z -= v.z;
    return *this;
}