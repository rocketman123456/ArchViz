#include "runtime/function/render/rhi/vulkan/utils/vulkan_buffer_utils.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_utils.h"
#include "runtime/function/render/rhi/vulkan/vulkan_device.h"

#include "runtime/core/base/macro.h"

namespace ArchViz
{
    void VulkanBufferUtils::createBufferVMA(std::shared_ptr<VulkanDevice> device, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, VmaAllocation& allocation)
    {
        VkBufferCreateInfo buffer_info {};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size  = size;
        buffer_info.usage = usage;

        VmaAllocationCreateInfo alloc_info {};
        alloc_info.usage = VMA_MEMORY_USAGE_AUTO;

        vmaCreateBuffer(device->m_assets_allocator, &buffer_info, &alloc_info, &buffer, &allocation, nullptr);
    }

    void VulkanBufferUtils::destroyBufferVMA(std::shared_ptr<VulkanDevice> device, VkBuffer& buffer, VmaAllocation& allocation) { vmaDestroyBuffer(device->m_assets_allocator, buffer, allocation); }

    void VulkanBufferUtils::createBuffer(std::shared_ptr<VulkanDevice> device,
                                         VkDeviceSize                  size,
                                         VkBufferUsageFlags            usage,
                                         VkMemoryPropertyFlags         properties,
                                         VkBuffer&                     buffer,
                                         VkDeviceMemory&               buffer_memory)
    {
        // fix warnings
        VkDeviceSize limit     = device->m_properties.limits.nonCoherentAtomSize;
        VkDeviceSize redundent = size % limit;
        if (redundent != 0)
        {
            VkDeviceSize free_size = limit - redundent;
            size += free_size;
        }

        // TODO : add all create info into function call
        VkBufferCreateInfo buffer_info {};
        buffer_info.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size        = size;
        buffer_info.usage       = usage;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device->m_device, &buffer_info, nullptr, &buffer) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create buffer!");
        }

        VkMemoryRequirements mem_requirements {};
        vkGetBufferMemoryRequirements(device->m_device, buffer, &mem_requirements);

        VkMemoryAllocateInfo alloc_info {};
        alloc_info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize  = mem_requirements.size;
        alloc_info.memoryTypeIndex = VulkanUtils::findMemoryType(device->m_physical_device, mem_requirements.memoryTypeBits, properties);

        if (vkAllocateMemory(device->m_device, &alloc_info, nullptr, &buffer_memory) != VK_SUCCESS)
        {
            LOG_FATAL("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(device->m_device, buffer, buffer_memory, 0);
    }

    void VulkanBufferUtils::copyBuffer(std::shared_ptr<VulkanDevice> device, VkCommandPool command_pool, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
    {
        VkCommandBufferAllocateInfo alloc_info {};
        alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool        = command_pool;
        alloc_info.commandBufferCount = 1;

        VkCommandBuffer command_buffer;
        vkAllocateCommandBuffers(device->m_device, &alloc_info, &command_buffer);

        VkCommandBufferBeginInfo begin_info {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(command_buffer, &begin_info);
        {
            VkBufferCopy copy_region {};
            copy_region.size = size;
            vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);
        }
        vkEndCommandBuffer(command_buffer);

        VkSubmitInfo submit_info {};
        submit_info.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers    = &command_buffer;

        vkQueueSubmit(device->m_transfer_queue, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(device->m_transfer_queue);

        vkFreeCommandBuffers(device->m_device, command_pool, 1, &command_buffer);
    }

    void VulkanBufferUtils::destroyBuffer(std::shared_ptr<VulkanDevice> device, VkBuffer& buffer, VkDeviceMemory& buffer_memory)
    {
        vkDestroyBuffer(device->m_device, buffer, nullptr);
        vkFreeMemory(device->m_device, buffer_memory, nullptr);

        buffer        = VK_NULL_HANDLE;
        buffer_memory = VK_NULL_HANDLE;
    }

    VkCommandBuffer VulkanBufferUtils::beginSingleTimeCommands(std::shared_ptr<VulkanDevice> device, VkCommandPool command_pool)
    {
        VkCommandBufferAllocateInfo alloc_info {};
        alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool        = command_pool;
        alloc_info.commandBufferCount = 1;

        VkCommandBuffer command_buffer;
        vkAllocateCommandBuffers(device->m_device, &alloc_info, &command_buffer);

        VkCommandBufferBeginInfo begin_info {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(command_buffer, &begin_info);

        return command_buffer;
    }

    void VulkanBufferUtils::endSingleTimeCommands(std::shared_ptr<VulkanDevice> device, VkCommandPool command_pool, VkCommandBuffer command_buffer)
    {
        vkEndCommandBuffer(command_buffer);

        VkSubmitInfo submit_info {};
        submit_info.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers    = &command_buffer;

        vkQueueSubmit(device->m_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(device->m_graphics_queue);

        vkFreeCommandBuffers(device->m_device, command_pool, 1, &command_buffer);
    }
} // namespace ArchViz
