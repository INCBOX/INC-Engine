#pragma once

namespace mathlib {

constexpr float PI = 3.14159265359f;

inline float DegToRad(float degrees) {
    return degrees * (PI / 180.0f);
}

inline float RadToDeg(float radians) {
    return radians * (180.0f / PI);
}

} // namespace mathlib
