#pragma once
#include "Ray.h"
#include "AABB.h"
#include "Sphere.h"
#include "Plane.h"
#include <limits>

namespace mathlib {

// Ray vs AABB intersection test using slab method
inline bool RayIntersectsAABB(const Ray& ray, const AABB& aabb, float* outT = nullptr) {
    float tmin = (aabb.min.x - ray.origin.x) / ray.direction.x;
    float tmax = (aabb.max.x - ray.origin.x) / ray.direction.x;

    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (aabb.min.y - ray.origin.y) / ray.direction.y;
    float tymax = (aabb.max.y - ray.origin.y) / ray.direction.y;

    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (aabb.min.z - ray.origin.z) / ray.direction.z;
    float tzmax = (aabb.max.z - ray.origin.z) / ray.direction.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    if (tmax < 0)
        return false;

    if (outT) *outT = tmin >= 0 ? tmin : tmax;
    return true;
}

// Ray vs Sphere intersection test
inline bool RayIntersectsSphere(const Ray& ray, const Sphere& sphere, float* outT = nullptr) {
    Vec3 m = ray.origin - sphere.center;
    float b = m.dot(ray.direction);
    float c = m.dot(m) - sphere.radius * sphere.radius;

    if (c > 0.0f && b > 0.0f) return false;

    float discr = b * b - c;

    if (discr < 0.0f) return false;

    float t = -b - std::sqrt(discr);

    if (t < 0) t = 0;

    if (outT) *outT = t;
    return true;
}

// Sphere vs AABB intersection test
inline bool SphereIntersectsAABB(const Sphere& sphere, const AABB& aabb) {
    float sqDist = 0.0f;
    const Vec3& c = sphere.center;

    if (c.x < aabb.min.x) {
        float d = c.x - aabb.min.x;
        sqDist += d * d;
    } else if (c.x > aabb.max.x) {
        float d = c.x - aabb.max.x;
        sqDist += d * d;
    }

    if (c.y < aabb.min.y) {
        float d = c.y - aabb.min.y;
        sqDist += d * d;
    } else if (c.y > aabb.max.y) {
        float d = c.y - aabb.max.y;
        sqDist += d * d;
    }

    if (c.z < aabb.min.z) {
        float d = c.z - aabb.min.z;
        sqDist += d * d;
    } else if (c.z > aabb.max.z) {
        float d = c.z - aabb.max.z;
        sqDist += d * d;
    }

    return sqDist <= sphere.radius * sphere.radius;
}

// Plane vs Sphere intersection test
inline bool PlaneIntersectsSphere(const Plane& plane, const Sphere& sphere) {
    float dist = plane.DistanceToPoint(sphere.center);
    return std::abs(dist) <= sphere.radius;
}

} // namespace mathlib
