#include "sse.h"
#include "vector.h"

// Example SSE-optimized dot product
float DotSSE(const Vector& a, const Vector& b) {
    __m128 va = _mm_set_ps(0, a.z, a.y, a.x);
    __m128 vb = _mm_set_ps(0, b.z, b.y, b.x);
    __m128 mul = _mm_mul_ps(va, vb);
    __m128 shuf = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(2,1,0,3));
    __m128 sums = _mm_add_ps(mul, shuf);
    sums = _mm_hadd_ps(sums, sums);
    float res;
    _mm_store_ss(&res, sums);
    return res;
}
