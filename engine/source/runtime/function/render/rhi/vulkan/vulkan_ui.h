#pragma once
#include "runtime/core/math/math_type.h"

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <memory>

namespace ArchViz
{
    class AssetManager;
    class ConfigManager;

    class VulkanInstance;
    class VulkanDevice;
    class VulkanTexture;
    class VulkanShader;
    class VulkanBuffer;

    class VulkanUI
    {
    public:
        void initialize();
        void clear();

        void prepareContext(float width, float height);
        void render();
        void recordCommandBuffer(VkCommandBuffer command_buffer, VkFramebuffer frame_buffer);

    private:
        void createRenderPass();
        void createDescriptorSetLayout();
        void createDescriptorSets();
        void createPipelineLayout();
        void createPipeline();

    public:
        struct PushConstantBlock
        {
            FVector2 scale;
            FVector2 translate;
        };

    public:
        GLFWwindow* m_window;

        std::shared_ptr<AssetManager>  m_asset_manager;
        std::shared_ptr<ConfigManager> m_config_manager;

        uint32_t m_image_count; // swap chain
        VkFormat m_image_format;

        std::shared_ptr<VulkanInstance> m_instance;
        std::shared_ptr<VulkanDevice>   m_device;
        std::shared_ptr<VulkanTexture>  m_font_texture;
        std::shared_ptr<VulkanShader>   m_shader;

        std::shared_ptr<VulkanBuffer> m_vertex_buffer;
        std::shared_ptr<VulkanBuffer> m_index_buffer;

        uint32_t m_vertex_count;
        uint32_t m_index_count;

        VkPipelineCache m_pipeline_cache;

        VkDescriptorPool      m_descriptor_pool;
        VkDescriptorSetLayout m_descriptor_set_layout;
        VkDescriptorSet       m_descriptor_set;

        VkPipelineLayout m_pipeline_layout;
        VkPipeline       m_pipeline;

        VkCommandPool   m_command_pool;
        VkCommandBuffer m_command_buffer;

        VkRenderPass m_ui_pass;

        PushConstantBlock m_push_const;
    };
} // namespace ArchViz
