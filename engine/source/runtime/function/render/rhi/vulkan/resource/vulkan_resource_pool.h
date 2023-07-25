#pragma once
#include "runtime/function/render/rhi/gpu_resources.h"

#include <volk.h>

namespace ArchViz
{
    class VulkanResourcePool
    {
    public:
        void accessBuffer();
        void accessTexture();
    };
} // namespace ArchViz
