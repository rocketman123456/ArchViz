#pragma once

#include <volk.h>

namespace ArchViz
{
    class VulkanBuffer
    {
    public:
        VkDevice               device;
        VkBuffer               buffer = VK_NULL_HANDLE;
        VkDeviceMemory         memory = VK_NULL_HANDLE;
        VkDescriptorBufferInfo descriptor;
        VkDeviceSize           size      = 0;
        VkDeviceSize           alignment = 0;
        void*                  mapped    = nullptr;
        /** @brief Usage flags to be filled by external source at buffer creation (to query at some later point) */
        VkBufferUsageFlags usageFlags;
        /** @brief Memory property flags to be filled by external source at buffer creation (to query at some later point) */
        VkMemoryPropertyFlags memoryPropertyFlags;

        // TODO
    };
} // namespace ArchViz
