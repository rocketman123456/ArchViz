#pragma once

#include <volk.h>

#include <memory>

namespace ArchViz
{
    class VulkanDevice;

    class VulkanCommandPool
    {
    public:
        void connect(std::shared_ptr<VulkanDevice> device);
        void initialize();
        void clear();

    public:
        std::shared_ptr<VulkanDevice> m_device;

        VkCommandPool m_command_pool;
    };
} // namespace ArchViz
