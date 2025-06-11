#pragma once
#include "Vec3.h"

namespace mathlib {

struct Ray {
    Vec3 origin;
    Vec3 direction; // should be normalized

    Ray() : origin(0,0,0), direction(0,0,1) {}
    Ray(const Vec3& origin_, const Vec3& direction_) : origin(origin_), direction(direction_.normalized()) {}

    Vec3 PointAt(float t) const {
        return origin + direction * t;
    }
};

} // namespace mathlib
