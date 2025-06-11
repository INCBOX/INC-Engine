#ifndef RAY_H
#define RAY_H

#include "vector.h"

class Ray_t
{
public:
    Vector m_Start;
    Vector m_Delta;
    Vector m_StartOffset;
    bool m_IsSwept;

    Ray_t() : m_IsSwept(false) {}

    void Init(const Vector& start, const Vector& end) {
        m_Start = start;
        m_Delta = end - start;
        m_IsSwept = (m_Delta.Length() != 0);
    }
};

#endif // RAY_H
