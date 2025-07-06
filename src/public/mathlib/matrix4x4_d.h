#pragma once
#include "mathlib/vector3_d.h"

class Matrix4x4_d {
public:
    // COLUMN-MAJOR: m[col][row]
    double m[4][4];

    Matrix4x4_d() {
        for (int col = 0; col < 4; ++col)
            for (int row = 0; row < 4; ++row)
                m[col][row] = 0.0;
    }

    double* operator[](int col) { return m[col]; }
    const double* operator[](int col) const { return m[col]; }

    const double* Data() const { return &m[0][0]; }

    static Matrix4x4_d Identity();
    static Matrix4x4_d Translation(const Vector3_d& t);
    static Matrix4x4_d LookAt(const Vector3_d& eye, const Vector3_d& center, const Vector3_d& up);
    static Matrix4x4_d Perspective(double fovYDegrees, double aspect, double nearZ, double farZ);
    static Matrix4x4_d Orthographic(double left, double right, double bottom, double top, double nearZ, double farZ);
	
	static Matrix4x4_d Translate(const Vector3_d& offset);
};

Matrix4x4_d operator*(const Matrix4x4_d& a, const Matrix4x4_d& b);
