#pragma once

#include "runtime/core/math/math_type.h"

namespace ArchViz
{
    class GraphicsUtils
    {
    public:
        static FMatrix4 lookAt(const FVector3& eye, const FVector3& center, const FVector3& up);

        static FMatrix4 perspective(float fov, float ratio, float znear, float zfar);

        static FMatrix4 orthogonal(float left, float right, float bottom, float top, float znear, float zfar);
    };
    
} // namespace ArchViz