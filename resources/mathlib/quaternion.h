#ifndef QUATERNION_H
#define QUATERNION_H

#include <cmath>

class Quaternion
{
public:
    float x, y, z, w;

    Quaternion() : x(0), y(0), z(0), w(1) {}
    Quaternion(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}

    void Normalize() {
        float len = sqrtf(x*x + y*y + z*z + w*w);
        x /= len; y /= len; z /= len; w /= len;
    }
};

#endif // QUATERNION_H
