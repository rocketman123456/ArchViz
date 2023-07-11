#pragma once

#include <volk.h>

#include <memory>

namespace ArchViz
{
    class VulkanDevice;

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