#pragma once

#include "runtime/function/render/rhi/vulkan/vulkan_struct.h"

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <optional>
#include <vector>

namespace ArchViz
{
    class VulkanSwapChain
    {
    public:
        VulkanSwapChain()  = default;
        ~VulkanSwapChain() = default;

        void connect(VkInstance instance, VkSurfaceKHR surface, VkPhysicalDevice physical_device, VkDevice device);
        void initialize(uint32_t width, uint32_t height, bool vsync = false, bool fullscreen = false);
        void clear();

        // VkResult acquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t* imageIndex);
        // VkResult queuePresent(VkQueue queue, uint32_t imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE);

    public:
        GLFWwindow*     m_window;
        VkSwapchainKHR  m_swap_chain = VK_NULL_HANDLE;
        VkExtent2D      m_swap_chain_extent;
        VkFormat        m_swap_chain_image_format;
        VkFormat        m_color_format;
        VkColorSpaceKHR m_color_space;
        uint32_t        m_image_count;
        uint32_t        m_queue_node_index = UINT32_MAX;

        QueueFamilyIndices m_indices;

        std::vector<VkImage>               m_images;
        std::vector<VulkanSwapChainBuffer> m_buffers;

    public:
        VkInstance       m_instance        = VK_NULL_HANDLE;
        VkSurfaceKHR     m_surface         = VK_NULL_HANDLE;
        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
        VkDevice         m_device          = VK_NULL_HANDLE;
    };
} // namespace ArchViz