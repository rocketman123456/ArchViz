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
        static void createBuffer(std::shared_ptr<VulkanDevice> device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory);

        static void copyBuffer(std::shared_ptr<VulkanDevice> device, VkCommandPool command_pool, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);
    };
} // namespace ArchViz
