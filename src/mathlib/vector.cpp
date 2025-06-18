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


/////
// MOVEMENT PHYSCIS: for faster length comparisons (avoid sqrt)
float Vector::LengthSqr() const {
    return x * x + y * y + z * z;
}

// useful for tiny velocity checks and optimization
bool Vector::IsZero(float epsilon) const {
    return LengthSqr() < epsilon * epsilon;
}

Vector& Vector::operator*=(float f) {
    x *= f; y *= f; z *= f;
    return *this;
}

Vector& Vector::operator/=(float f) {
    if (f != 0.0f) {
        x /= f; y /= f; z /= f;
    }
    return *this;
}

// used for friction, sliding, etc)
Vector Vector::ProjectOnto(const Vector& other) const {
    float otherLenSqr = other.LengthSqr();
    if (otherLenSqr == 0.0f) return Vector(0, 0, 0);
    float dot = this->Dot(other);
    return other * (dot / otherLenSqr);
}
/////