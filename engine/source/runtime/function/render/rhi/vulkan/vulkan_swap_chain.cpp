#include "runtime/function/render/rhi/vulkan/vulkan_swap_chain.h"
#include "runtime/function/render/rhi/vulkan/vulkan_device.h"
#include "runtime/function/render/rhi/vulkan/vulkan_instance.h"
#include "runtime/function/render/rhi/vulkan/vulkan_rhi.h"
#include "runtime/function/render/rhi/vulkan/vulkan_texture_utils.h"
#include "runtime/function/render/rhi/vulkan/vulkan_utils.h"

#include "runtime/core/base/macro.h"

#include <set>

namespace ArchViz
{
    void VulkanSwapChain::connect(std::shared_ptr<VulkanInstance> instance, std::shared_ptr<VulkanDevice> device)
    {
        m_instance = instance;
        m_device   = device;
    }

    void VulkanSwapChain::initialize(uint32_t width, uint32_t height, bool vsync, bool fullscreen)
    {
        // create swap chain
        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_device->m_physical_device, m_instance->m_surface, &format_count, NULL);
        ASSERT(format_count > 0);

        std::vector<VkSurfaceFormatKHR> surface_formats(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_device->m_physical_device, m_instance->m_surface, &format_count, surface_formats.data());

        if ((format_count == 1) && (surface_formats[0].format == VK_FORMAT_UNDEFINED))
        {
            m_color_format = VK_FORMAT_B8G8R8A8_UNORM;
            m_color_space  = surface_formats[0].colorSpace;
        }
        else
        {
            // iterate over the list of available surface format and
            // check for the presence of VK_FORMAT_B8G8R8A8_UNORM
            bool found_B8G8R8A8_UNORM = false;
            for (auto&& surface_format : surface_formats)
            {
                if (surface_format.format == VK_FORMAT_B8G8R8A8_UNORM)
                {
                    m_color_format       = surface_format.format;
                    m_color_space        = surface_format.colorSpace;
                    found_B8G8R8A8_UNORM = true;
                    break;
                }
            }

            // in case VK_FORMAT_B8G8R8A8_UNORM is not available
            // select the first available color format
            if (!found_B8G8R8A8_UNORM)
            {
                m_color_format = surface_formats[0].format;
                m_color_space  = surface_formats[0].colorSpace;
            }
        }

        SwapChainSupportDetails swap_chain_support = VulkanUtils::querySwapChainSupport(m_device->m_physical_device, m_instance->m_surface);

        VkSurfaceFormatKHR surface_format = VulkanUtils::chooseSwapSurfaceFormat(swap_chain_support.formats);
        VkPresentModeKHR   present_mode   = VulkanUtils::chooseSwapPresentMode(swap_chain_support.presentModes);
        VkExtent2D         extent         = VulkanUtils::chooseSwapExtent(swap_chain_support.capabilities, m_window);

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
        create_info.imageFormat      = surface_format.format;
        create_info.imageColorSpace  = surface_format.colorSpace;
        create_info.imageExtent      = extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        m_indices                            = VulkanUtils::findQueueFamilies(m_device->m_physical_device, m_instance->m_surface);
        std::set<uint32_t>    queue_families = {m_indices.m_graphics_family.value(), m_indices.m_present_family.value(), m_indices.m_compute_family.value()};
        std::vector<uint32_t> families;
        for (auto family : queue_families)
        {
            families.push_back(family);
        }

        if (families.size() > 1)
        {
            create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = static_cast<uint32_t>(families.size());
            create_info.pQueueFamilyIndices   = families.data();
        }
        else
        {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        create_info.preTransform   = swap_chain_support.capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode    = present_mode;
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

        m_swap_chain_image_format = surface_format.format;
        m_swap_chain_extent       = extent;

        m_buffers.resize(m_images.size());

        for (size_t i = 0; i < m_images.size(); i++)
        {
            m_buffers[i].view = VulkanTextureUtils::createImageView(m_device, m_images[i], m_swap_chain_image_format);
        }
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
