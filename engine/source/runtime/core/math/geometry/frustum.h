#pragma once

#include "runtime/core/math/math_type.h"
#include "runtime/core/math/matrix/matrix4.h"
#include "runtime/core/math/vector/vector4.h"

#include <array>

namespace ArchViz
{
    class Frustum
    {
    public:
        enum side
        {
            LEFT   = 0,
            RIGHT  = 1,
            TOP    = 2,
            BOTTOM = 3,
            BACK   = 4,
            FRONT  = 5
        };

        std::array<FVector4, 6> planes;

        void update(const FMatrix4& matrix);
        bool checkSphere(const FVector3& pos, float radius);
    };
} // namespace ArchViz
