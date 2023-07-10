#pragma once

#include <volk.h>

#include <memory>

namespace ArchViz
{
    class AssetManager;
    class ConfigManager;
    class VulkanDevice;
    class VulkanShader;

    class VulkanPipeline
    {
    public:
        void setShaderModule(std::shared_ptr<VulkanShader> shader);
        void setDevice(std::shared_ptr<VulkanDevice> device);
        void setRenderPass(VkRenderPass render_pass);
        void setDescriptorSetLayout(VkDescriptorSetLayout layout);

        void initialize();
        void clear();

    public:
        std::shared_ptr<VulkanDevice> m_device;
        std::shared_ptr<VulkanShader> m_shader;

        VkRenderPass m_render_pass;

        VkPipelineLayout      m_pipeline_layout;
        VkPipeline            m_pipeline;
        VkDescriptorSetLayout m_descriptor_set_layout;
    };
} // namespace ArchViz