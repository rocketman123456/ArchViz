#pragma once

#include <volk.h>

#include <optional>
#include <vector>

namespace ArchViz
{
    struct VulkanConstants
    {
        static const std::vector<const char*> validation_layers;
        static const std::vector<const char*> device_extensions;
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> m_graphics_family;
        std::optional<uint32_t> m_present_family;
        std::optional<uint32_t> m_compute_family;
        std::optional<uint32_t> m_transfer_family;

        bool isComplete() { return m_graphics_family.has_value() && m_present_family.has_value() && m_compute_family.has_value(); }
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR        capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR>   presentModes;
    };

    struct VulkanSwapChainBuffer
    {
        VkImage     image;
        VkImageView view;
    };
} // namespace ArhViz
