#pragma once
#include "runtime/core/meta/reflection/reflection.h"

#include <string>

namespace ArchViz
{
    REFLECTION_TYPE(LightRes)
    CLASS(LightRes, Fields)
    {
        REFLECTION_BODY(LightRes);

    public:
        std::string m_name;

        float m_pos_x;
        float m_pos_y;
        float m_pos_z;
        float m_color_r;
        float m_color_g;
        float m_color_b;
    };
} // namespace ArchViz
