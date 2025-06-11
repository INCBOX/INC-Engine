#pragma once
#include "Vec3.h"

namespace mathlib {

// Clamp a value between min and max
inline float Clamp(float val, float minVal, float maxVal) {
    if (val < minVal) return minVal;
    if (val > maxVal) return maxVal;
    return val;
}

// Linear interpolation between a and b
inline float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

// Axis-Aligned Bounding Box struct
struct AABB {
    Vec3 min;
    Vec3 max;

    bool Intersects(const AABB& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }

    // Check if point inside AABB
    bool ContainsPoint(const Vec3& point) const {
        return (point.x >= min.x && point.x <= max.x) &&
               (point.y >= min.y && point.y <= max.y) &&
               (point.z >= min.z && point.z <= max.z);
    }
};

// Sphere struct for simple sphere collisions
struct Sphere {
    Vec3 center;
    float radius;

    bool Intersects(const Sphere& other) const {
        float distSq = (center - other.center).Dot(center - other.center);
        float radiusSum = radius + other.radius;
        return distSq <= radiusSum * radiusSum;
    }

    bool ContainsPoint(const Vec3& point) const {
        float distSq = (center - point).Dot(center - point);
        return distSq <= radius * radius;
    }
};

// Simple Ray struct
struct Ray {
    Vec3 origin;
    Vec3 direction; // should be normalized

    // Ray-AABB intersection (returns true if intersects, tNear is distance)
    bool Intersects(const AABB& box, float& tNear) const {
        float tmin = (box.min.x - origin.x) / direction.x;
        float tmax = (box.max.x - origin.x) / direction.x;

        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (box.min.y - origin.y) / direction.y;
        float tymax = (box.max.y - origin.y) / direction.y;

        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
            tmin = tymin;

        if (tymax < tmax)
            tmax = tymax;

        float tzmin = (box.min.z - origin.z) / direction.z;
        float tzmax = (box.max.z - origin.z) / direction.z;

        if (tzmin > tzmax) std::swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;

        if (tzmax < tmax)
            tmax = tzmax;

        tNear = tmin;

        return tmax >= 0;
    }

    // Ray-Sphere intersection test
    bool Intersects(const Sphere& sphere, float& t) const {
        Vec3 m = origin - sphere.center;
        float b = m.Dot(direction);
        float c = m.Dot(m) - sphere.radius * sphere.radius;

        if (c > 0.0f && b > 0.0f)
            return false;

        float discr = b*b - c;
        if (discr < 0.0f)
            return false;

        t = -b - std::sqrt(discr);
        if (t < 0) t = 0;
        return true;
    }
};

} // namespace mathlib
