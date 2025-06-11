#ifndef SSE_H
#define SSE_H

#include <xmmintrin.h>

inline __m128 VectorLoad(const float* p) {
    return _mm_load_ps(p);
}

inline void VectorStore(float* p, __m128 v) {
    _mm_store_ps(p, v);
}

#endif // SSE_H
