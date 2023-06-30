#pragma once
#include "runtime/function/render/rhi/rhi.h"

#include "runtime/function/render/rhi/vulkan/vulkan_swap_chain.h"

#include <volk.h>

#include <cstdint>
#include <memory>
#include <vector>

namespace ArchViz
{
    class AssetManager;
    class ConfigManager;
    class VulkanDevice;
    class VulkanSwapChain;

    class VulkanRHI : public RHI
    {
    public:
        VulkanRHI()          = default;
        virtual ~VulkanRHI() = default;

        void setConfigManager(std::shared_ptr<ConfigManager> config_manager) override;
        void setAssetManager(std::shared_ptr<AssetManager> asset_manager) override;

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

    public:
        std::shared_ptr<AssetManager>  m_asset_manager;
        std::shared_ptr<ConfigManager> m_config_manager;

    private:
        RHIInitInfo m_initialize_info;

        VkInstance               m_instance;
        VkDebugUtilsMessengerEXT m_debug_messenger;

        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
        VkDevice         m_device          = VK_NULL_HANDLE;

        QueueFamilyIndices m_indices;

        VkSurfaceKHR m_surface;

        VkQueue m_graphics_queue;
        VkQueue m_compute_queue;
        VkQueue m_present_queue;

        std::shared_ptr<VulkanDevice>    m_vulkan_device;
        std::shared_ptr<VulkanSwapChain> m_vulkan_swap_chain;

        VkPipelineLayout m_pipeline_layout;

        const std::vector<const char*> m_validation_layers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char*> m_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        const bool m_enable_validation_layers = false;
    };
} // namespace ArchViz
