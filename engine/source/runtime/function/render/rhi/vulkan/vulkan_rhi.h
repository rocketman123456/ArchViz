#pragma once
#include "runtime/function/render/rhi/rhi.h"

#include <volk.h>

#include <cstdint>
#include <optional>
#include <vector>

namespace ArchViz
{
    class VulkanDevice;
    class VulkanSwapChain;

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
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapChain();
        void createImageViews();
        void createGraphicsPipeline();

    private:
        RHIInitInfo m_initialize_info;

        VkInstance               m_instance;
        VkDebugUtilsMessengerEXT m_debug_messenger;

        VkPhysicalDevice              m_physical_device = VK_NULL_HANDLE;
        std::shared_ptr<VulkanDevice> m_vulkan_device;

        QueueFamilyIndices m_indices;

        VkDevice m_device;

        VkSurfaceKHR m_surface;

        VkQueue m_graphics_queue;
        VkQueue m_compute_queue;
        VkQueue m_present_queue;

        VkSwapchainKHR m_swap_chain;
        VkFormat       m_swap_chain_image_format;
        VkExtent2D     m_swap_chain_extent;

        std::vector<VkImage>     m_swap_chain_images;
        std::vector<VkImageView> m_swap_chain_image_views;

        const std::vector<const char*> m_validation_layers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char*> m_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        const bool m_enable_validation_layers = false;
    };
} // namespace ArchViz
