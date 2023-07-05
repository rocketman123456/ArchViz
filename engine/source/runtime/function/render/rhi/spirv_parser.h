#pragma once

#include "runtime/function/render/rhi/gpu_resources.h"

#include <volk.h>

#include <spirv/1.0/spirv.h>

#include <string>
#include <vector>

namespace ArchViz
{
    namespace SPIRV
    {
        constexpr uint32_t MAX_SET_COUNT = 32;

        struct ParseResult
        {
            uint32_t                    set_count;
            DescriptorSetLayoutCreation sets[MAX_SET_COUNT];
        };

        VkShaderStageFlags parse_execution_model(SpvExecutionModel model);

        void parse_binary(const uint32_t* data, size_t data_size, const std::vector<uint8_t>& name_buffer, ParseResult* parse_result);
    } // namespace SPIRV
} // namespace ArchViz
