#pragma once
#include "runtime/core/math/math_type.h"

#include <volk.h>

#include <string>

namespace ArchViz
{
    class VulkanDebugUtils
    {
    public:
        static void setup(VkInstance instance);
        static void cmdBeginLabel(VkCommandBuffer cmdbuffer, const std::string& caption, const FVector4& color);
        static void cmdEndLabel(VkCommandBuffer cmdbuffer);
    };
}
