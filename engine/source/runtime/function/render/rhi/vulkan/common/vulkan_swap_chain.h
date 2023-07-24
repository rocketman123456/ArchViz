#pragma once

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <memory>
#include <optional>
#include <vector>

namespace ArchViz
{
    class VulkanInstance;
    class VulkanDevice;

    struct VulkanSwapChainBuffer
    {
        VkImage     image;
        VkImageView view;
    };

    class VulkanSwapChain
    {
    public:
        void initialize(uint32_t width, uint32_t height, bool vsync = false, bool fullscreen = false);
        void clear();

        VkResult acquireNextImage(VkSemaphore present_complete_semaphore, uint32_t* image_index);
        VkResult queuePresent(VkQueue queue, uint32_t image_index, VkSemaphore wait_semaphore = VK_NULL_HANDLE);

    public:
        GLFWwindow*        m_window;
        VkSwapchainKHR     m_swap_chain = VK_NULL_HANDLE;
        VkExtent2D         m_swap_chain_extent;
        VkSurfaceFormatKHR m_surface_format;
        VkPresentModeKHR   m_present_mode;
        uint32_t           m_image_count;
        uint32_t           m_queue_node_index = UINT32_MAX;

        std::vector<VkImage>               m_images;
        std::vector<VulkanSwapChainBuffer> m_buffers;

    public:
        std::shared_ptr<VulkanInstance> m_instance;
        std::shared_ptr<VulkanDevice>   m_device;
    };
} // namespace ArchViz