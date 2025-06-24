#pragma once

#include "mathlib/vector3_f.h"
#include "mathlib/vector3_d.h"
#include "mathlib/matrix4x4_f.h"
#include "mathlib/matrix4x4_d.h"

namespace precision {
// Vector3
Vector3_f ToFloat(const Vector3_d& v);
Vector3_d ToDouble(const Vector3_f& v);

// Matrix4x4
// Convert double precision matrix to float precision matrix
Matrix4x4_f ToFloat(const Matrix4x4_d& m);
// Convert float precision matrix to double precision matrix
Matrix4x4_d ToDouble(const Matrix4x4_f& m);

// Future...
// Quaternion_f ToFloat(const Quaternion_d& q);
// etc.

} // namespace precision