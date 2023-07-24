#pragma once
#include "runtime/core/meta/reflection/reflection.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_shader.h"

#include <volk.h>

#include <memory>
#include <string>

namespace ArchViz
{
    class AssetManager;
    class ConfigManager;
    class VulkanDevice;
    class VulkanShader;

    class DescriptorSetLayoutCreateInfo
    {
    public:
    };

    REFLECTION_TYPE(VulkanPipelineCreateInfo)
    CLASS(VulkanPipelineCreateInfo, Fields)
    {
        REFLECTION_BODY(VulkanPipelineCreateInfo)
    public:
        ShaderModuleCreateInfo m_shader_info;

        std::string m_name;
    };

    class VulkanPipeline
    {
    public:
        void initialize();
        void clear();

    private:
        void createDescriptorSetLayout();
        void createPipeline();

    public:
        std::shared_ptr<VulkanDevice> m_device;
        std::shared_ptr<VulkanShader> m_shader;

        VkPipelineCache m_pipeline_cache;
        VkRenderPass    m_render_pass;

        VkPipelineLayout m_pipeline_layout;
        VkPipeline       m_pipeline;

        VkDescriptorSetLayout m_descriptor_set_layout;
    };
} // namespace ArchViz