#pragma once
#include "runtime/core/meta/reflection/reflection.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_pipeline.h"

#include <volk.h>

#include <memory>
#include <string>
#include <vector>

namespace ArchViz
{
    class VulkanDevice;

    REFLECTION_TYPE(VulkanRenderPassCreateInfo)
    CLASS(VulkanRenderPassCreateInfo, Fields)
    {
        REFLECTION_BODY(VulkanRenderPassCreateInfo)
    public:
        std::vector<VulkanPipelineCreateInfo> m_pipeline_infos;

        std::string m_name;
    };

    class VulkanRenderPass
    {
    public:
        void initialize();
        void clear();

    public:
        std::shared_ptr<VulkanDevice> m_device;

        VkRenderPass m_render_pass;

        VkFormat m_color_format;
    };
} // namespace ArchViz