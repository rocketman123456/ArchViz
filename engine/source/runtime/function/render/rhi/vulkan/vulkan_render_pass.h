#pragma once

#include <volk.h>

#include <memory>

namespace ArchViz
{
    class VulkanDevice;

    class VulkanRenderPass
    {
    public:
        void setDevice(std::shared_ptr<VulkanDevice> device);

        void initialize();
        void clear();

    public:
        VkRenderPass m_render_pass;

        VkFormat m_color_format;

    private:
        std::shared_ptr<VulkanDevice> m_device;
    };
} // namespace ArchViz