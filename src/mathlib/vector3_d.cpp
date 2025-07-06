#include <cmath>
#include "mathlib/vector3_d.h"

double Vector3_d::Dot(const Vector3_d& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vector3_d Vector3_d::Cross(const Vector3_d& other) const {
    return Vector3_d(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

Vector3_d Vector3_d::Normalize() const {
    double len = std::sqrt(x * x + y * y + z * z);
    if (len == 0.0) return Vector3_d(0, 0, 0);
    return Vector3_d(x / len, y / len, z / len);
}

double Vector3_d::Length() const {
    return std::sqrt(x * x + y * y + z * z);
}

double Vector3_d::LengthSqr() const {
    return x * x + y * y + z * z;
}

bool Vector3_d::IsZero(double epsilon) const {
    return LengthSqr() < epsilon * epsilon;
}

Vector3_d& Vector3_d::operator*=(double f) {
    x *= f; y *= f; z *= f;
    return *this;
}

Vector3_d& Vector3_d::operator/=(double f) {
    if (f != 0.0) {
        x /= f; y /= f; z /= f;
    }
    return *this;
}

Vector3_d Vector3_d::ProjectOnto(const Vector3_d& other) const {
    double otherLenSqr = other.LengthSqr();
    if (otherLenSqr == 0.0) return Vector3_d(0, 0, 0);
    double dot = this->Dot(other);
    return other * (dot / otherLenSqr);
}