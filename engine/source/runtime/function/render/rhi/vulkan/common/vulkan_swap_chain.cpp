#include "runtime/function/render/rhi/vulkan/common/vulkan_swap_chain.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_device.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_instance.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_texture_utils.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_utils.h"

#include "runtime/core/base/macro.h"

#include <set>

namespace ArchViz
{
    void VulkanSwapChain::initialize(uint32_t width, uint32_t height, bool vsync, bool fullscreen)
    {
        // create swap chain
        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_device->m_physical_device, m_instance->m_surface, &format_count, NULL);
        ASSERT(format_count > 0);

        SwapChainSupportDetails swap_chain_support = VulkanUtils::querySwapChainSupport(m_device->m_physical_device, m_instance->m_surface);

        m_surface_format    = VulkanUtils::chooseSwapSurfaceFormat(swap_chain_support.formats);
        m_present_mode      = VulkanUtils::chooseSwapPresentMode(swap_chain_support.presentModes);
        m_swap_chain_extent = VulkanUtils::chooseSwapExtent(swap_chain_support.capabilities, m_window);

        m_image_count = swap_chain_support.capabilities.minImageCount + 1;
        if (swap_chain_support.capabilities.maxImageCount > 0 && m_image_count > swap_chain_support.capabilities.maxImageCount)
        {
            m_image_count = swap_chain_support.capabilities.maxImageCount;
        }

        VkExtent2D swapchainExtent = {};
        // If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
        if (swap_chain_support.capabilities.currentExtent.width == (uint32_t)-1)
        {
            // If the surface size is undefined, the size is set to
            // the size of the images requested.
            m_swap_chain_extent.width  = width;
            m_swap_chain_extent.height = height;
        }
        else
        {
            // If the surface size is defined, the swap chain size must match
            m_swap_chain_extent = swap_chain_support.capabilities.currentExtent;
        }

        VkSwapchainCreateInfoKHR create_info {};
        create_info.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface          = m_instance->m_surface;
        create_info.minImageCount    = m_image_count;
        create_info.imageFormat      = m_surface_format.format;
        create_info.imageColorSpace  = m_surface_format.colorSpace;
        create_info.imageExtent      = m_swap_chain_extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t indices[] = {m_device->m_indices.m_graphics_family.value(), m_device->m_indices.m_present_family.value()};

        if (m_device->m_indices.m_graphics_family.value() != m_device->m_indices.m_present_family.value())
        {
            create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices   = indices;
        }
        else
        {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        create_info.preTransform   = swap_chain_support.capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode    = m_present_mode;
        create_info.clipped        = VK_TRUE;

        VkSwapchainKHR old_swap_chain = m_swap_chain;
        create_info.oldSwapchain      = old_swap_chain;

        if (vkCreateSwapchainKHR(m_device->m_device, &create_info, nullptr, &m_swap_chain) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create swap chain!");
        }

        // create image views
        if (old_swap_chain != VK_NULL_HANDLE)
        {
            for (uint32_t i = 0; i < m_image_count; i++)
            {
                vkDestroyImageView(m_device->m_device, m_buffers[i].view, nullptr);
            }
            vkDestroySwapchainKHR(m_device->m_device, old_swap_chain, nullptr);
        }

        vkGetSwapchainImagesKHR(m_device->m_device, m_swap_chain, &m_image_count, nullptr);
        m_images.resize(m_image_count);
        vkGetSwapchainImagesKHR(m_device->m_device, m_swap_chain, &m_image_count, m_images.data());

        m_buffers.resize(m_images.size());

        for (size_t i = 0; i < m_images.size(); i++)
        {
            m_buffers[i].view = VulkanTextureUtils::createImageView(m_device, m_images[i], m_surface_format.format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
        }
    }

    VkResult VulkanSwapChain::acquireNextImage(VkSemaphore present_complete_semaphore, uint32_t* image_index)
    {
        VkResult result = vkAcquireNextImageKHR(m_device->m_device, m_swap_chain, UINT64_MAX, present_complete_semaphore, (VkFence) nullptr, image_index);
        return result;
    }

    VkResult VulkanSwapChain::queuePresent(VkQueue queue, uint32_t image_index, VkSemaphore wait_semaphore)
    {
        VkPresentInfoKHR present_info {};
        present_info.sType          = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.pNext          = nullptr;
        present_info.swapchainCount = 1;
        present_info.pSwapchains    = &m_swap_chain;
        present_info.pImageIndices  = &image_index;
        // Check if a wait semaphore has been specified to wait for before presenting the image
        if (wait_semaphore != VK_NULL_HANDLE)
        {
            present_info.pWaitSemaphores    = &wait_semaphore;
            present_info.waitSemaphoreCount = 1;
        }
        VkResult result = vkQueuePresentKHR(queue, &present_info);
        return result;
    }

    void VulkanSwapChain::clear()
    {
        if (m_swap_chain != VK_NULL_HANDLE)
        {
            for (uint32_t i = 0; i < m_image_count; i++)
            {
                vkDestroyImageView(m_device->m_device, m_buffers[i].view, nullptr);
                vkDestroyImage(m_device->m_device, m_buffers[i].image, nullptr);
            }

            vkDestroySwapchainKHR(m_device->m_device, m_swap_chain, nullptr);
            m_swap_chain = VK_NULL_HANDLE;
        }
    }
} // namespace ArchViz
