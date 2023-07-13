#pragma once

#include <volk.h>

#include <vk_mem_alloc.h>

namespace ArchViz
{
    class VulkanBuffer
    {
    public:
        VkDevice               device;
        VkBuffer               buffer = VK_NULL_HANDLE;
        VkDeviceMemory         memory = VK_NULL_HANDLE;
        VkDescriptorBufferInfo descriptor;
        VkDeviceSize           size   = 0;
        void*                  mapped = nullptr;

        VkDeviceSize          alignment = 0;
        VkBufferUsageFlags    usage;    /** @brief Usage flags to be filled by external source at buffer creation (to query at some later point) */
        VkMemoryPropertyFlags property; /** @brief Memory property flags to be filled by external source at buffer creation (to query at some later point) */

        VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void     unmap();
        VkResult bind(VkDeviceSize offset = 0);
        void     setupDescriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void     copyTo(void* data, VkDeviceSize size);
        VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void     destroy();
    };

    class VulkanBufferVMA : public VulkanBuffer
    {
    public:
        VmaAllocator m_allocator = nullptr;
    };
} // namespace ArchViz
