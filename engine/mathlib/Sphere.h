#pragma once
#include "Vec3.h"
#include <cmath>

namespace mathlib {

struct Sphere {
    Vec3 center;
    float radius;

    Sphere() : center(0,0,0), radius(1.0f) {}
    Sphere(const Vec3& c, float r) : center(c), radius(r) {}

    bool Contains(const Vec3& point) const {
        Vec3 diff = point - center;
        return diff.length() <= radius;
    }

    bool Intersects(const Sphere& other) const {
        float rSum = radius + other.radius;
        Vec3 diff = center - other.center;
        return diff.length() <= rSum;
    }
};

} // namespace mathlib
