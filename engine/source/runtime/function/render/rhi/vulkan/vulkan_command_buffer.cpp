#include "runtime/function/render/rhi/vulkan/vulkan_command_buffer.h"

#include "runtime/core/base/macro.h"

namespace ArchViz
{
    void VulkanCommandBuffer::connect(VkDevice device, VkCommandPool command_pool)
    {
        ASSERT(device);
        m_device = device;
        ASSERT(command_pool);
        m_command_pool = command_pool;
    }

    void VulkanCommandBuffer::initialize()
    {
        VkCommandBufferAllocateInfo alloc_info {};
        alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool        = m_command_pool;
        alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(m_device, &alloc_info, &m_command_buffer) != VK_SUCCESS)
        {
            LOG_FATAL("failed to allocate command buffer!");
        }
    }

    void VulkanCommandBuffer::clear() {}
} // namespace ArchViz
