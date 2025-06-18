#include <cmath>
#include "mathlib/vector.h"

float Vector::Dot(const Vector& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vector Vector::Cross(const Vector& other) const {
    return Vector(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

Vector Vector::Normalize() const {
    float len = std::sqrt(x * x + y * y + z * z);
    if (len == 0.0f) return Vector(0, 0, 0);
    return Vector(x / len, y / len, z / len);
}
// CAMERA, PLAYER, STUFF
float Vector::Length() const {
    return std::sqrt(x * x + y * y + z * z);
}