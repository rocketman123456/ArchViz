#pragma once
#include "runtime/function/render/rhi/rhi.h"

#include <volk.h>

#include <vk_mem_alloc.h>

#include <array>
#include <unordered_map>
#include <vector>

namespace ArchViz
{
    class VulkanShaderUtils
    {
    public:
        static VkShaderModule createShaderModule(VkDevice device, const std::string& shader_file);
        static VkShaderModule createShaderModule(VkDevice device, const std::vector<unsigned char>& shader_code);

    private:
        static std::unordered_map<uint32_t, VkSampler> m_mipmap_sampler_map;
        static VkSampler                               m_nearest_sampler;
        static VkSampler                               m_linear_sampler;
    };
} // namespace ArchViz
