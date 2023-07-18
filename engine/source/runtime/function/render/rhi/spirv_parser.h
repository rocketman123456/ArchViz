// https://github.com/KhronosGroup/SPIRV-Reflect
#pragma once
#include "runtime/function/render/rhi/gpu_resources.h"

#include <volk.h>

#include <spirv/unified1/spirv.h>

#include <cstdint>
#include <string>
#include <vector>

namespace ArchViz
{
    namespace SPIRV
    {
        constexpr uint32_t MAX_SET_COUNT = 32;

        struct ParseResult
        {
            uint32_t                    set_count {0};
            DescriptorSetLayoutCreation sets[MAX_SET_COUNT] {};
        };

        VkShaderStageFlags parse_execution_model(SpvExecutionModel model);

        void parse_binary(const std::vector<uint32_t>& data, std::string& name_buffer, ParseResult& parse_result);
    } // namespace SPIRV
} // namespace ArchViz
