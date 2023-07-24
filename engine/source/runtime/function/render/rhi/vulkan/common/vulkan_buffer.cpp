#include "runtime/function/render/rhi/vulkan/common/vulkan_buffer.h"
#include "runtime/function/render/rhi/vulkan/vulkan_device.h"

#include "runtime/core/base/macro.h"

namespace ArchViz
{
    VkResult VulkanBuffer::map(VkDeviceSize size, VkDeviceSize offset) { return vkMapMemory(device->m_device, memory, offset, size, 0, &mapped); }

    void VulkanBuffer::unmap()
    {
        if (mapped)
        {
            vkUnmapMemory(device->m_device, memory);
            mapped = nullptr;
        }
    }

    VkResult VulkanBuffer::bind(VkDeviceSize offset) { return vkBindBufferMemory(device->m_device, buffer, memory, offset); }

    void VulkanBuffer::setupDescriptor(VkDeviceSize size, VkDeviceSize offset)
    {
        descriptor.offset = offset;
        descriptor.buffer = buffer;
        descriptor.range  = size;
    }

    void VulkanBuffer::copyTo(void* data, VkDeviceSize size)
    {
        ASSERT(mapped);
        memcpy(mapped, data, size);
    }

    VkResult VulkanBuffer::flush(VkDeviceSize size, VkDeviceSize offset)
    {
        VkMappedMemoryRange mapped_range = {};
        mapped_range.sType               = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mapped_range.memory              = memory;
        mapped_range.offset              = offset;
        mapped_range.size                = size;
        return vkFlushMappedMemoryRanges(device->m_device, 1, &mapped_range);
    }

    VkResult VulkanBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
    {
        VkMappedMemoryRange mapped_range = {};
        mapped_range.sType               = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mapped_range.memory              = memory;
        mapped_range.offset              = offset;
        mapped_range.size                = size;
        return vkInvalidateMappedMemoryRanges(device->m_device, 1, &mapped_range);
    }

    void VulkanBuffer::destroy()
    {
        if (mapped)
        {
            unmap();
        }
        if (buffer)
        {
            vkDestroyBuffer(device->m_device, buffer, nullptr);
        }
        if (memory)
        {
            vkFreeMemory(device->m_device, memory, nullptr);
        }
    }
} // namespace ArchViz
