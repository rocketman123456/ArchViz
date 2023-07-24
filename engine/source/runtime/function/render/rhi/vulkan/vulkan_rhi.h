#pragma once
#include "runtime/function/render/rhi/rhi.h"
#include "runtime/function/render/rhi/vulkan/vulkan_struct.h"

#include "runtime/function/render/geometry/light.h"
#include "runtime/function/render/geometry/particle.h"
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
    class VulkanBuffer;
    class Vertex;

    struct UBO
    {
        alignas(16) FMatrix4 model;
        alignas(16) FMatrix4 view;
        alignas(16) FMatrix4 proj;
    };

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

        // for debug
        void setFPS(uint32_t fps) override;

        void clear() override;

        void createSwapChain() override;
        void recreateSwapChain() override;

    private:
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------

        void createInstance();

        void createVulkanDevice();

        void createDescriptorPool();
        void createBindlessDescriptorPool();

        // void createDescriptorSetLayout();
        void createBindlessDescriptorSetLayout();

        void createRenderPass();
        void createGraphicsPipelineCache();
        void createGraphicsPipeline();

        void createCommandPool();

        void createDepthResources();
        void createFramebuffers();

        void createCommandBuffer();

        // TODO : move to scene part
        void createTextureImage();
        // TODO : move to scene part
        void loadModel();

        void createVertexBuffer();
        void createIndexBuffer();
        void createUniformBuffers();
        void createDescriptorSets();

        void createBindlessDescriptorSets();

        void createSyncObjects();

        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------

        void createComputeDescriptorSetLayout();
        void createComputePipeline();
        void createShaderStorageBuffers();
        void createComputeDescriptorSets();
        void createComputeCommandBuffers();
        void createComputeUniformBuffers();
        void createComputeSyncObjects();

        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------

        void createImGui();

    private:
        void recordComputeCommandBuffer(VkCommandBuffer commandBuffer);
        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t image_index);
        void updateUniformBuffer(uint32_t current_image);
        void drawFrame();

    public:
        const bool m_enable_validation_layers = true;

        const uint32_t k_max_frames_in_flight = 2;

        const uint32_t k_particle_count = 8192;

        const uint32_t k_global_pool_elements     = 128;
        const uint32_t k_bindless_texture_binding = 10;
        const uint32_t k_max_bindless_resources   = 1024;

        bool m_framebuffer_resized = false;
        bool m_bindless_supported  = true;

        uint32_t m_fps;

        std::shared_ptr<AssetManager>  m_asset_manager;  // for debug, or use global
        std::shared_ptr<ConfigManager> m_config_manager; // for debug, or use global

        RHIInitInfo m_initialize_info;

        std::shared_ptr<VulkanInstance>   m_vulkan_instance;
        std::shared_ptr<VulkanDevice>     m_vulkan_device;
        std::shared_ptr<VulkanSwapChain>  m_vulkan_swap_chain;
        std::shared_ptr<VulkanPipeline>   m_vulkan_pipeline;
        std::shared_ptr<VulkanRenderPass> m_vulkan_render_pass;

        std::shared_ptr<VulkanTexture> m_vulkan_texture;

        std::shared_ptr<VulkanTexture> m_vulkan_texture_ui;

        std::shared_ptr<VulkanUI> m_vulkan_ui;

        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------

        // TODO add pipeline cache storage to increase creation speed
        VkPipelineCache m_pipeline_cache;

        VkFormat       m_depth_format;
        VkImage        m_depth_image;
        VkDeviceMemory m_depth_image_memory;
        VkImageView    m_depth_image_view;

        std::vector<VkFramebuffer> m_swap_chain_framebuffers;

        std::vector<VkSemaphore> m_image_available_semaphores;
        std::vector<VkSemaphore> m_render_finished_semaphores;
        std::vector<VkFence>     m_in_flight_fences;
        uint32_t                 m_current_frame = 0;

        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------

        uint32_t              m_bindless_pool_size;
        VkDescriptorPool      m_bindless_pool;
        VkDescriptorSetLayout m_bindless_set_layout;
        VkDescriptorSet       m_bindless_set;

        VkDescriptorPool m_descriptor_pool;

        std::vector<VkDescriptorSet> m_descriptor_sets;

        std::vector<VkBuffer>       m_uniform_buffers;
        std::vector<VkDeviceMemory> m_uniform_buffers_memory;
        std::vector<void*>          m_uniform_buffers_mapped;

        std::vector<VkBuffer>       m_uniform_light_buffers;
        std::vector<VkDeviceMemory> m_uniform_light_buffers_memory;
        std::vector<void*>          m_uniform_light_buffers_mapped;

        std::shared_ptr<VulkanBuffer> m_vulkan_vertex_buffer;
        std::shared_ptr<VulkanBuffer> m_vulkan_index_buffer;

        // VkBuffer       m_vertex_buffer;
        // VkDeviceMemory m_vertex_buffer_memory;
        // VkBuffer       m_index_buffer;
        // VkDeviceMemory m_index_buffer_memory;

        // The main purpose of a command pool is to be a (mostly) single-threaded allocator for the storage used by a set of command buffers which are filled by that thread.
        VkCommandPool                m_command_pool;
        std::vector<VkCommandBuffer> m_command_buffers;

        VkCommandBuffer m_transfer_buffer;

        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------

        // TODO : add a simple compute pass
        VkDescriptorSetLayout        m_compute_descriptor_set_layout;
        std::vector<VkDescriptorSet> m_compute_descriptor_sets;

        VkPipelineLayout m_compute_pipeline_layout;
        VkPipeline       m_compute_pipeline;

        std::vector<VkCommandBuffer> m_compute_command_buffers;

        std::vector<VkBuffer>       m_shader_storage_buffers;
        std::vector<VkDeviceMemory> m_shader_storage_buffers_memory;

        // TODO : add particle ubo
        std::vector<VkBuffer>       m_particle_uniform_buffers;
        std::vector<VkDeviceMemory> m_particle_uniform_buffers_memory;
        std::vector<void*>          m_particle_uniform_buffers_mapped;

        std::vector<VkSemaphore> m_compute_finished_semaphores;
        std::vector<VkFence>     m_compute_in_flight_fences;

        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------
        // ---------------------------------------------------------------------------

        std::vector<Vertex>   m_vertices;
        std::vector<uint32_t> m_indices;

    public:
        UBO   m_ubo;
        Light m_light_ubo;
        float m_dt_ubo;

        bool m_minimize {false};
    };
} // namespace ArchViz
