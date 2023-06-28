#pragma once
#include "runtime/function/render/rhi/rhi.h"

#include <volk.h>

#include <cstdint>
#include <optional>
#include <vector>

namespace ArchViz
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> m_graphics_family;
        std::optional<uint32_t> m_present_family;
        std::optional<uint32_t> m_compute_family;

        bool isComplete() { return m_graphics_family.has_value() && m_present_family.has_value() && m_compute_family.has_value(); }
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR        capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR>   presentModes;
    };

    class VulkanRHI : public RHI
    {
    public:
        VulkanRHI()          = default;
        virtual ~VulkanRHI() = default;

        void initialize(RHIInitInfo initialize_info) override;
        void prepareContext() override;

        void clear() override;

    private:
        void createInstance();
        void setupDebugMessenger();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSurface();

    private:
        RHIInitInfo m_initialize_info;

        VkInstance               m_instance;
        VkDebugUtilsMessengerEXT m_debug_messenger;

        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;

        QueueFamilyIndices m_indices;

        VkDevice m_device;

        VkSurfaceKHR m_surface;

        VkQueue m_graphics_queue;
        VkQueue m_compute_queue;
        VkQueue m_present_queue;

        const std::vector<const char*> m_validation_layers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char*> m_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        const bool m_enable_validation_layers = false;
    };
} // namespace ArchViz
