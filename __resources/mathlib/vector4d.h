#ifndef VECTOR4D_H
#define VECTOR4D_H

class Vector4D
{
public:
    float x, y, z, w;

    Vector4D() : x(0), y(0), z(0), w(0) {}
    Vector4D(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}
};

#endif // VECTOR4D_H
