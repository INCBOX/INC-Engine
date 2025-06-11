#pragma once
#include "Vec3.h"
#include <cmath>

namespace mathlib {

struct Plane {
    Vec3 normal;
    float d; // distance from origin along normal

    Plane() : normal(0, 1, 0), d(0) {}

    Plane(const Vec3& normal_, float d_) : normal(normal_), d(d_) {}

    // Construct from 3 points (counter-clockwise)
    Plane(const Vec3& a, const Vec3& b, const Vec3& c) {
        normal = (b - a).cross(c - a).normalized();
        d = -normal.dot(a);
    }

    void Normalize() {
        float len = normal.length();
        if (len == 0) return;
        normal = normal / len;
        d /= len;
    }

    // Distance from point to plane (positive if in front)
    float DistanceToPoint(const Vec3& point) const {
        return normal.dot(point) + d;
    }
};

} // namespace mathlib
