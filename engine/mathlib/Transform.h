#pragma once
#include "Vec3.h"
#include "Quaternion.h"
#include "Mat4.h"

namespace mathlib {

struct Transform {
    Vec3 position;
    Quaternion rotation;
    Vec3 scale;

    Transform()
        : position(0,0,0), rotation(Quaternion::Identity()), scale(1,1,1)
    {}

    Mat4 GetModelMatrix() const {
        Mat4 T = Mat4::Translation(position);
        Mat4 R = rotation.ToMatrix();
        Mat4 S = Mat4::Scale(scale);
        return T * R * S;
    }
};

} // namespace mathlib
