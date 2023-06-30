#pragma once
#include "runtime/function/render/rhi/rhi.h"

#include "runtime/function/render/rhi/vulkan/vulkan_struct.h"
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

    private:
        std::shared_ptr<AssetManager>  m_asset_manager;
        std::shared_ptr<ConfigManager> m_config_manager;

        RHIInitInfo m_initialize_info;

        VkInstance               m_instance;
        VkDebugUtilsMessengerEXT m_debug_messenger;
        VkSurfaceKHR             m_surface;

        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
        VkDevice         m_device          = VK_NULL_HANDLE;

        // QueueFamilyIndices m_indices;

        // VkQueue m_graphics_queue;
        // VkQueue m_compute_queue;
        // VkQueue m_present_queue;

        std::shared_ptr<VulkanDevice>    m_vulkan_device;
        std::shared_ptr<VulkanSwapChain> m_vulkan_swap_chain;

        VkPipelineLayout m_pipeline_layout;

        const bool m_enable_validation_layers = false;
    };
} // namespace ArchViz
