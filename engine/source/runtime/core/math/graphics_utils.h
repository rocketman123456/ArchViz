#pragma once

#include "runtime/core/math/math_type.h"

namespace ArchViz
{
    FMatrix4 look_at(const FVector3& eye, const FVector3& center, const FVector3& up);

    FMatrix4 perspective(float fov, float ratio, float znear, float zfar);

    FMatrix4 orthogonal(float left, float right, float bottom, float top, float znear, float zfar);
} // namespace ArchViz