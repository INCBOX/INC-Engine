#pragma once
#include "vector.h"

class VMatrix {
public:
    float m[4][4];

    void Identity();
    void Perspective(float fovY, float aspect, float zNear, float zFar);
    void LookAt(const Vector& eye, const Vector& target, const Vector& up);
};