#include <cmath>
#include "mathlib/vector3_f.h"

float Vector3_f::Dot(const Vector3_f& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vector3_f Vector3_f::Cross(const Vector3_f& other) const {
    return Vector3_f(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

Vector3_f Vector3_f::Normalize() const {
    float len = std::sqrt(x * x + y * y + z * z);
    if (len == 0.0f) return Vector3_f(0, 0, 0);
    return Vector3_f(x / len, y / len, z / len);
}

// CAMERA, PLAYER, STUFF
float Vector3_f::Length() const {
    return std::sqrt(x * x + y * y + z * z);
}

// MOVEMENT PHYSCIS: for faster length comparisons (avoid sqrt)
float Vector3_f::LengthSqr() const {
    return x * x + y * y + z * z;
}

// useful for tiny velocity checks and optimization
bool Vector3_f::IsZero(float epsilon) const {
    return LengthSqr() < epsilon * epsilon;
}

Vector3_f& Vector3_f::operator*=(float f) {
    x *= f; y *= f; z *= f;
    return *this;
}

Vector3_f& Vector3_f::operator/=(float f) {
    if (f != 0.0f) {
        x /= f; y /= f; z /= f;
    }
    return *this;
}

// used for friction, sliding, etc)
Vector3_f Vector3_f::ProjectOnto(const Vector3_f& other) const {
    float otherLenSqr = other.LengthSqr();
    if (otherLenSqr == 0.0f) return Vector3_f(0, 0, 0);
    float dot = this->Dot(other);
    return other * (dot / otherLenSqr);
}