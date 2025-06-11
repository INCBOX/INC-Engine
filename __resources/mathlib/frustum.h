#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "plane.h"

class Frustum
{
public:
    cplane_t planes[6]; // left, right, top, bottom, near, far
};

#endif // FRUSTUM_H
