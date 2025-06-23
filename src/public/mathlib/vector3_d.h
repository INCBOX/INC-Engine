#pragma once

class Vector3_d {
public:
    double x, y, z;

    Vector3_d() : x(0.0), y(0.0), z(0.0) {}
    Vector3_d(double X, double Y, double Z) : x(X), y(Y), z(Z) {}

    Vector3_d operator+(const Vector3_d& v) const { return Vector3_d(x + v.x, y + v.y, z + v.z); }
    Vector3_d operator-(const Vector3_d& v) const { return Vector3_d(x - v.x, y - v.y, z - v.z); }
    Vector3_d operator*(double f) const { return Vector3_d(x * f, y * f, z * f); }

    Vector3_d& operator+=(const Vector3_d& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vector3_d& operator-=(const Vector3_d& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vector3_d& operator*=(double f);
    Vector3_d& operator/=(double f);
    Vector3_d ProjectOnto(const Vector3_d& other) const;

    double* Base() { return &x; }
    const double* Base() const { return &x; }
    double Length() const;
    double LengthSqr() const;
    double Dot(const Vector3_d& other) const;
    Vector3_d Cross(const Vector3_d& other) const;
    Vector3_d Normalize() const;

    bool IsZero(double epsilon = 1e-12) const;
};

using Vec3_d = Vector3_d;
