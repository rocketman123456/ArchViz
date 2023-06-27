#pragma once
#include "runtime/core/meta/reflection/reflection.h"

#include "runtime/platform/file_system/basic/file_system.h"

#include <string>
#include <vector>

namespace ArchViz
{
    // stroe mount points
    REFLECTION_TYPE(VFSConfig)
    CLASS(VFSConfig, Fields)
    {
        REFLECTION_BODY(VFSConfig)
    public:
        std::vector<FSConfig> m_configs;
    };
} // namespace ArchViz
