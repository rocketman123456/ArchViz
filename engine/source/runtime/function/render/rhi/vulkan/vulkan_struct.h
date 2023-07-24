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

        static constexpr uint32_t k_max_frames_in_flight = 2;

        static constexpr uint32_t k_particle_count = 8192;

        static constexpr uint32_t k_global_pool_elements     = 128;
        static constexpr uint32_t k_bindless_texture_binding = 10;
        static constexpr uint32_t k_max_bindless_resources   = 1024;
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> m_graphics_family;
        std::optional<uint32_t> m_present_family;
        std::optional<uint32_t> m_compute_family;
        std::optional<uint32_t> m_transfer_family;

        bool isComplete() { return m_graphics_family.has_value() && m_present_family.has_value() && m_compute_family.has_value() && m_transfer_family.has_value(); }
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR        capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR>   presentModes;
    };

} // namespace ArchViz
