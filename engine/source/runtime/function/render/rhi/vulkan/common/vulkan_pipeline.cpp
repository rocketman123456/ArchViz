#include "runtime/function/render/rhi/vulkan/common/vulkan_pipeline.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_device.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_shader.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_utils.h"

#include "runtime/function/render/geometry/vertex.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/core/base/macro.h"

namespace ArchViz
{
    void VulkanPipeline::initialize()
    {
        m_shader->initialize();

        createDescriptorSetLayout();
        createPipeline();

        m_shader->clear();
    }

    void VulkanPipeline::createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding ubo_layout_binding {};
        ubo_layout_binding.binding            = 0;
        ubo_layout_binding.descriptorCount    = 1;
        ubo_layout_binding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        ubo_layout_binding.pImmutableSamplers = nullptr;
        ubo_layout_binding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding sampler_layout_binding {};
        sampler_layout_binding.binding            = 1;
        sampler_layout_binding.descriptorCount    = 1;
        sampler_layout_binding.descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        sampler_layout_binding.pImmutableSamplers = nullptr;
        sampler_layout_binding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutBinding light_layout_binding {};
        light_layout_binding.binding            = 2;
        light_layout_binding.descriptorCount    = 1;
        light_layout_binding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        light_layout_binding.pImmutableSamplers = nullptr;
        light_layout_binding.stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 3> bindings = {ubo_layout_binding, sampler_layout_binding, light_layout_binding};

        VkDescriptorSetLayoutCreateInfo layout_info {};
        layout_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
        layout_info.pBindings    = bindings.data();

        if (vkCreateDescriptorSetLayout(m_device->m_device, &layout_info, nullptr, &m_descriptor_set_layout) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create descriptor set layout!");
        }
    }

    void VulkanPipeline::createPipeline()
    {
        // TODO : add custom bindings
        auto binding_description    = Vertex::getBindingDescription();
        auto attribute_descriptions = Vertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertex_input_info {};
        vertex_input_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount   = 1;
        vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
        vertex_input_info.pVertexBindingDescriptions      = &binding_description;
        vertex_input_info.pVertexAttributeDescriptions    = attribute_descriptions.data();

        VkPipelineInputAssemblyStateCreateInfo input_assembly {};
        input_assembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewport_state {};
        viewport_state.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount  = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer {};
        rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable        = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth               = 1.0f;
        rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable         = VK_FALSE;
        rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;

        VkPipelineMultisampleStateCreateInfo multisampling {};
        multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable  = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState color_blend_attachment {};
        color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        color_blend_attachment.blendEnable    = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo color_blending {};
        color_blending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.logicOpEnable     = VK_FALSE;
        color_blending.logicOp           = VK_LOGIC_OP_COPY;
        color_blending.attachmentCount   = 1;
        color_blending.pAttachments      = &color_blend_attachment;
        color_blending.blendConstants[0] = 0.0f;
        color_blending.blendConstants[1] = 0.0f;
        color_blending.blendConstants[2] = 0.0f;
        color_blending.blendConstants[3] = 0.0f;

        VkPipelineDepthStencilStateCreateInfo depth_stencil {};
        depth_stencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil.depthTestEnable       = VK_TRUE;
        depth_stencil.depthWriteEnable      = VK_TRUE;
        depth_stencil.depthCompareOp        = VK_COMPARE_OP_LESS;
        depth_stencil.depthBoundsTestEnable = VK_FALSE;
        depth_stencil.stencilTestEnable     = VK_FALSE;
        depth_stencil.minDepthBounds        = 0.0f; // Optional
        depth_stencil.maxDepthBounds        = 1.0f; // Optional
        depth_stencil.front                 = {};   // Optional
        depth_stencil.back                  = {};   // Optional

        std::vector<VkDynamicState>      dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        VkPipelineDynamicStateCreateInfo dynamic_state {};
        dynamic_state.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
        dynamic_state.pDynamicStates    = dynamic_states.data();

        VkPipelineLayoutCreateInfo pipeline_layout_info {};
        pipeline_layout_info.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = 1;
        pipeline_layout_info.pSetLayouts    = &m_descriptor_set_layout;

        if (vkCreatePipelineLayout(m_device->m_device, &pipeline_layout_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create pipeline layout!")
        }

        VkGraphicsPipelineCreateInfo pipeline_info {};
        pipeline_info.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount          = static_cast<uint32_t>(m_shader->m_stage_info.size());
        pipeline_info.pStages             = m_shader->m_stage_info.data();
        pipeline_info.pVertexInputState   = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState      = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState   = &multisampling;
        pipeline_info.pColorBlendState    = &color_blending;
        pipeline_info.pDepthStencilState  = &depth_stencil;
        pipeline_info.pDynamicState       = &dynamic_state;
        pipeline_info.layout              = m_pipeline_layout;
        pipeline_info.renderPass          = m_render_pass;
        pipeline_info.basePipelineHandle  = VK_NULL_HANDLE;
        pipeline_info.subpass             = 0;

        if (vkCreateGraphicsPipelines(m_device->m_device, m_pipeline_cache, 1, &pipeline_info, nullptr, &m_pipeline) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create graphics pipeline!");
        }
    }

    void VulkanPipeline::clear()
    {
        vkDestroyPipeline(m_device->m_device, m_pipeline, nullptr);
        vkDestroyPipelineLayout(m_device->m_device, m_pipeline_layout, nullptr);
        vkDestroyDescriptorSetLayout(m_device->m_device, m_descriptor_set_layout, nullptr);
    }
} // namespace ArchViz