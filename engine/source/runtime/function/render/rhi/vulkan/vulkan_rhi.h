#pragma once
#include "runtime/function/render/rhi/rhi.h"

#include "runtime/function/render/rhi/vulkan/vulkan_struct.h"

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vk_mem_alloc.h>

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
    class VulkanInstance;
    class VulkanUI;

    class VulkanRHI : public RHI
    {
    public:
        VulkanRHI()          = default;
        virtual ~VulkanRHI() = default;

        void setConfigManager(std::shared_ptr<ConfigManager> config_manager) override;
        void setAssetManager(std::shared_ptr<AssetManager> asset_manager) override;

        void initialize(RHIInitInfo initialize_info) override;
        void prepareContext() override;
        void render() override;

        void clear() override;

        void createSwapChain() override;
        void recreateSwapChain() override;

    private:
        void createInstance();

        void createVulkanDevice();

        void createDescriptorSetLayout();
        void createDescriptorPool();

        void createRenderPass();
        void createGraphicsPipeline();

        void createFramebuffers();

        void createCommandPool();
        void createCommandBuffer();

        void createImGui();

        void createVertexBuffer();
        void createIndexBuffer();
        void createUniformBuffers();
        void createDescriptorSets();

        void createSyncObjects();

        void createAssetAllocator();

    private:
        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
        void updateUniformBuffer(uint32_t current_image);
        void drawFrame();

    private:
        const bool m_enable_validation_layers = true;
        const int  MAX_FRAMES_IN_FLIGHT       = 2;

        bool m_framebuffer_resized = false;

        std::shared_ptr<AssetManager>  m_asset_manager;
        std::shared_ptr<ConfigManager> m_config_manager;

        RHIInitInfo m_initialize_info;

        std::shared_ptr<VulkanInstance>   m_vulkan_instance;
        std::shared_ptr<VulkanDevice>     m_vulkan_device;
        std::shared_ptr<VulkanSwapChain>  m_vulkan_swap_chain;
        std::shared_ptr<VulkanPipeline>   m_vulkan_pipeline;
        std::shared_ptr<VulkanRenderPass> m_vulkan_render_pass;

        std::shared_ptr<VulkanUI> m_vulkan_ui;

        std::vector<VkFramebuffer> m_swap_chain_framebuffers;

        VkDescriptorSetLayout        m_descriptor_set_layout;
        VkDescriptorPool             m_descriptor_pool;
        std::vector<VkDescriptorSet> m_descriptor_sets;

        std::vector<VkBuffer>       m_uniform_buffers;
        std::vector<VkDeviceMemory> m_uniform_buffers_memory;
        std::vector<void*>          m_uniform_buffers_mapped;

        // asset allocator use VMA library
        VmaAllocator m_assets_allocator;

        VkBuffer       m_vertex_buffer;
        VkDeviceMemory m_vertex_buffer_memory;
        VkBuffer       m_index_buffer;
        VkDeviceMemory m_index_buffer_memory;

        VkCommandPool m_command_pool;
        // VkCommandBuffer m_command_buffer;
        std::vector<VkCommandBuffer> m_command_buffers;

        VkCommandBuffer m_transfer_buffer;

        std::vector<VkSemaphore> m_image_available_semaphores;
        std::vector<VkSemaphore> m_render_finished_semaphores;
        std::vector<VkFence>     m_in_flight_fences;
        uint32_t                 m_current_frame = 0;
    };
} // namespace ArchViz
