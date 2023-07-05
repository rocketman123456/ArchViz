#pragma once

#include <volk.h>

namespace ArchViz
{
    class VulkanCommandBuffer
    {
    public:
        void connect(VkDevice device, VkCommandPool command_pool);
        void initialize();
        void clear();

    public:
        VkDevice      m_device;
        VkCommandPool m_command_pool;

        VkCommandBuffer m_command_buffer;
    };
} // namespace ArchViz
