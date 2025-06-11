#pragma once
#include "Vec3.h"
#include <algorithm>

namespace mathlib {

struct AABB {
    Vec3 min;
    Vec3 max;

    AABB() 
        : min( std::numeric_limits<float>::max() )
        , max( -std::numeric_limits<float>::max() ) 
    {}

    AABB(const Vec3& min_, const Vec3& max_)
        : min(min_), max(max_) 
    {}

    bool Contains(const Vec3& point) const {
        return (point.x >= min.x && point.x <= max.x) &&
               (point.y >= min.y && point.y <= max.y) &&
               (point.z >= min.z && point.z <= max.z);
    }

    bool Intersects(const AABB& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }

    void ExpandToInclude(const Vec3& point) {
        min.x = std::min(min.x, point.x);
        min.y = std::min(min.y, point.y);
        min.z = std::min(min.z, point.z);

        max.x = std::max(max.x, point.x);
        max.y = std::max(max.y, point.y);
        max.z = std::max(max.z, point.z);
    }

    Vec3 GetCenter() const {
        return (min + max) * 0.5f;
    }

    Vec3 GetSize() const {
        return max - min;
    }
};

} // namespace mathlib
