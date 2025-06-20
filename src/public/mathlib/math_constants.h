#pragma once

namespace math {
    constexpr double PI  = 3.14159265358979323846;
    constexpr double TAU = 6.28318530717958647692;

    constexpr double DEG2RAD(double deg) { return deg * (PI / 180.0); }
    constexpr double RAD2DEG(double rad) { return rad * (180.0 / PI); }
}