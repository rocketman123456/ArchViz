#pragma once
#include "runtime/function/render/rhi/rhi.h"
#include "runtime/function/render/rhi/vulkan/vulkan_struct.h"

#include "runtime/function/render/geometry/vertex.h"

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vk_mem_alloc.h>

#include <array>
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
    class VulkanTexture;
    class Vertex;

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

        void createCommandPool();

        void createDepthResources();
        void createFramebuffers();

        void createCommandBuffer();

        void createTextureImage();

        void loadModel();

        void createVertexBuffer();
        void createIndexBuffer();
        void createUniformBuffers();
        void createDescriptorSets();

        void createSyncObjects();

        void createImGui();

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

        std::shared_ptr<VulkanTexture> m_vulkan_texture;

        std::shared_ptr<VulkanUI> m_vulkan_ui;

        std::vector<VkFramebuffer> m_swap_chain_framebuffers;

        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------

        VkFormat       m_depth_format;
        VkImage        m_depth_image;
        VkDeviceMemory m_depth_image_memory;
        VkImageView    m_depth_image_view;

        VkDescriptorSetLayout        m_descriptor_set_layout;
        VkDescriptorPool             m_descriptor_pool;
        std::vector<VkDescriptorSet> m_descriptor_sets;

        std::vector<VkBuffer>       m_uniform_buffers;
        std::vector<VkDeviceMemory> m_uniform_buffers_memory;
        std::vector<void*>          m_uniform_buffers_mapped;

        VkBuffer       m_vertex_buffer;
        VkDeviceMemory m_vertex_buffer_memory;
        VkBuffer       m_index_buffer;
        VkDeviceMemory m_index_buffer_memory;

        // The main purpose of a command pool is to be a (mostly) single-threaded allocator for the storage used by a set of command buffers which are filled by that thread.
        VkCommandPool                m_command_pool;
        std::vector<VkCommandBuffer> m_command_buffers;

        VkCommandBuffer m_transfer_buffer;

        std::vector<VkSemaphore> m_image_available_semaphores;
        std::vector<VkSemaphore> m_render_finished_semaphores;
        std::vector<VkFence>     m_in_flight_fences;
        uint32_t                 m_current_frame = 0;

        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------

        std::vector<Vertex>   m_vertices;
        std::vector<uint32_t> m_indices;
    };
} // namespace ArchViz
