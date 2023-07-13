#pragma once

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vk_mem_alloc.h>

#include <memory>

namespace ArchViz
{
    class VulkanDevice;

    class VulkanBufferUtils
    {
    public:
        static void createBufferVMA(std::shared_ptr<VulkanDevice> device, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, VmaAllocation& allocation);

        static void destroyBufferVMA(std::shared_ptr<VulkanDevice> device, VkBuffer& buffer, VmaAllocation& allocation);

        static void createBuffer(std::shared_ptr<VulkanDevice> device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory);

        static void copyBuffer(std::shared_ptr<VulkanDevice> device, VkCommandPool command_pool, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

        static void destroyBuffer(std::shared_ptr<VulkanDevice> device, VkBuffer& buffer, VkDeviceMemory& buffer_memory);

        static VkCommandBuffer beginSingleTimeCommands(std::shared_ptr<VulkanDevice> device, VkCommandPool command_pool);

        static void endSingleTimeCommands(std::shared_ptr<VulkanDevice> device, VkCommandPool command_pool, VkCommandBuffer command_buffer);
    };
} // namespace ArchViz
