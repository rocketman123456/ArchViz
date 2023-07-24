#include "runtime/function/render/rhi/vulkan/common/vulkan_command_pool.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_device.h"

#include "runtime/core/base/macro.h"

namespace ArchViz
{
    void VulkanCommandPool::connect(std::shared_ptr<VulkanDevice> device)
    {
        ASSERT(device);
        m_device = device;
    }

    void VulkanCommandPool::initialize()
    {
        VkCommandPoolCreateInfo pool_info {};
        pool_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = m_device->m_indices.m_graphics_family.value();

        if (vkCreateCommandPool(m_device->m_device, &pool_info, nullptr, &m_command_pool) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create command pool!");
        }
    }

    void VulkanCommandPool::clear() { vkDestroyCommandPool(m_device->m_device, m_command_pool, nullptr); }
} // namespace ArchViz