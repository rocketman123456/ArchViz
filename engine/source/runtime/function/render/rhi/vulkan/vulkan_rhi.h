#pragma once
#include "runtime/function/render/rhi/rhi.h"

#include "runtime/function/render/rhi/vulkan/vulkan_struct.h"

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
    class VulkanPipeline;
    class VulkanRenderPass;

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
        void createRenderPass();
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

        std::shared_ptr<VulkanDevice>    m_vulkan_device;
        std::shared_ptr<VulkanSwapChain> m_vulkan_swap_chain;

        VkRenderPass     m_render_pass;
        VkPipelineLayout m_pipeline_layout;
        VkPipeline       m_pipeline;

        const bool m_enable_validation_layers = false;
    };
} // namespace ArchViz
