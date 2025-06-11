#ifndef FLOATING_ORIGIN_H
#define FLOATING_ORIGIN_H

#include "vector3d.h"

// Simple floating origin manager
class FloatingOrigin
{
public:
    Vector3d origin;

    FloatingOrigin() : origin(0,0,0) {}

    // Rebase origin to new center
    void RebaseOrigin(const Vector3d& newCenter) {
        origin = newCenter;
    }

    // Convert world position to local position relative to origin
    Vector3d WorldToLocal(const Vector3d& worldPos) const {
        return worldPos - origin;
    }

    // Convert local position to world position
    Vector3d LocalToWorld(const Vector3d& localPos) const {
        return localPos + origin;
    }
};

#endif // FLOATING_ORIGIN_H
