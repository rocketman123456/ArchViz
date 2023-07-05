#pragma once
#include "runtime/core/meta/reflection/reflection.h"

#include "runtime/core/math/vector/vector3.h"
#include "runtime/core/math/vector/vector4.h"

namespace ArchViz
{
    REFLECTION_TYPE(ColorRGB)
    CLASS(ColorRGB, Fields)
    {
        REFLECTION_BODY(ColorRGB);

    public:
        float r;
        float g;
        float b;

        Vector3 toVector3() const { return Vector3(r, g, b); }
    };

    REFLECTION_TYPE(ColorRGBA)
    CLASS(ColorRGBA, Fields)
    {
        REFLECTION_BODY(ColorRGBA);

    public:
        float r;
        float g;
        float b;
        float a;

        Vector4 toVector4() const { return Vector4(r, g, b, a); }
    };
} // namespace ArchViz
