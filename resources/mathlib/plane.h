#ifndef PLANE_H
#define PLANE_H

#include "vector.h"

class cplane_t
{
public:
    Vector normal;
    float dist;

    cplane_t() : dist(0) {}
};

#endif // PLANE_H
