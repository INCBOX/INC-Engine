// mathlib_precision.h
#pragma once

#ifdef MATHLIB_USE_DOUBLE
    #include "mathlib/vector3_d.h"
    #include "mathlib/matrix4x4_d.h"
    using Vector3 = Vector3_d;
    using Matrix4x4 = Matrix4x4_d;
#else
    #include "mathlib/vector3_f.h"
    #include "mathlib/matrix4x4_f.h"
    using Vector3 = Vector3_f;
    using Matrix4x4 = Matrix4x4_f;
#endif