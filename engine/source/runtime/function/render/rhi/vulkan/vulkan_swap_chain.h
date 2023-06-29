#pragma once

#include <volk.h>

#include <vector>

namespace ArchViz
{
    struct VulkanSwapChainBuffer
    {
        VkImage     image;
        VkImageView view;
    };

    class VulkanSwapChain
    {
    public:
        VulkanSwapChain();
        ~VulkanSwapChain();

        // void     initSurface();
        // void     connect(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device);
        // void     create(uint32_t* width, uint32_t* height, bool vsync = false, bool fullscreen = false);
        // VkResult acquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t* imageIndex);
        // VkResult queuePresent(VkQueue queue, uint32_t imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE);
        // void     cleanup();

    public:
        VkFormat        m_color_format;
        VkColorSpaceKHR m_color_space;
        VkSwapchainKHR  m_swap_chain = VK_NULL_HANDLE;
        uint32_t        m_image_count;
        uint32_t        m_queue_node_index = UINT32_MAX;

        std::vector<VkImage>               m_images;
        std::vector<VulkanSwapChainBuffer> m_buffers;

    private:
        VkInstance       m_instance;
        VkDevice         m_device;
        VkPhysicalDevice m_physical_device;
        VkSurfaceKHR     m_surface;

        // Function pointers
        PFN_vkGetPhysicalDeviceSurfaceSupportKHR      fpGetPhysicalDeviceSurfaceSupportKHR;
        PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
        PFN_vkGetPhysicalDeviceSurfaceFormatsKHR      fpGetPhysicalDeviceSurfaceFormatsKHR;
        PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
        PFN_vkCreateSwapchainKHR                      fpCreateSwapchainKHR;
        PFN_vkDestroySwapchainKHR                     fpDestroySwapchainKHR;
        PFN_vkGetSwapchainImagesKHR                   fpGetSwapchainImagesKHR;
        PFN_vkAcquireNextImageKHR                     fpAcquireNextImageKHR;
        PFN_vkQueuePresentKHR                         fpQueuePresentKHR;
    };
} // namespace ArchViz