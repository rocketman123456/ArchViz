#pragma once
#include "runtime/core/meta/reflection/reflection.h"

#include <string>
#include <vector>

namespace ArchViz
{
    REFLECTION_TYPE(FSConfig)
    CLASS(FSConfig, Fields)
    {
        REFLECTION_BODY(FSConfig)
    public:
        std::string m_vpath;
        std::string m_rpath;
        std::string m_type;
    };

    // stroe mount points
    REFLECTION_TYPE(VFSConfig)
    CLASS(VFSConfig, Fields)
    {
        REFLECTION_BODY(VFSConfig)
    public:
        std::vector<FSConfig> m_configs;
    };
} // namespace ArchViz
