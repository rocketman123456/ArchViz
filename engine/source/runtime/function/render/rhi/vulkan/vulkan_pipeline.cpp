#include "runtime/function/render/rhi/vulkan/vulkan_pipeline.h"
#include "runtime/function/render/rhi/vulkan/vulkan_device.h"
#include "runtime/function/render/rhi/vulkan/vulkan_shader.h"
#include "runtime/function/render/rhi/vulkan/vulkan_utils.h"

#include "runtime/function/render/geometry/vertex.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/core/base/macro.h"

namespace ArchViz
{
    void VulkanPipeline::setShaderModule(std::shared_ptr<VulkanShader> shader)
    {
        ASSERT(shader);
        m_shader = shader;
    }

    void VulkanPipeline::setDevice(std::shared_ptr<VulkanDevice> device)
    {
        ASSERT(device);
        m_device = device;
    }

    void VulkanPipeline::setRenderPass(VkRenderPass render_pass)
    {
        ASSERT(render_pass);
        m_render_pass = render_pass;
    }

    void VulkanPipeline::initialize()
    {
        m_shader->initialize();

        // TODO : make auto
        VkPipelineShaderStageCreateInfo vert_shader_stage_info {};
        vert_shader_stage_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vert_shader_stage_info.stage  = VK_SHADER_STAGE_VERTEX_BIT;
        vert_shader_stage_info.module = m_shader->m_vert_shader;
        vert_shader_stage_info.pName  = "main";

        VkPipelineShaderStageCreateInfo frag_shader_stage_info {};
        frag_shader_stage_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        frag_shader_stage_info.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
        frag_shader_stage_info.module = m_shader->m_frag_shader;
        frag_shader_stage_info.pName  = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vert_shader_stage_info, frag_shader_stage_info};

        // TODO : add custom bindings
        auto bindingDescription    = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertex_input_info {};
        vertex_input_info.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount   = 1;
        vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertex_input_info.pVertexBindingDescriptions      = &bindingDescription;
        vertex_input_info.pVertexAttributeDescriptions    = attributeDescriptions.data();

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
        rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable         = VK_FALSE;

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

        std::vector<VkDynamicState>      dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        VkPipelineDynamicStateCreateInfo dynamic_state {};
        dynamic_state.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
        dynamic_state.pDynamicStates    = dynamic_states.data();

        VkPipelineLayoutCreateInfo pipeline_layout_info {};
        pipeline_layout_info.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount         = 0;
        pipeline_layout_info.pushConstantRangeCount = 0;

        if (vkCreatePipelineLayout(m_device->m_device, &pipeline_layout_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create pipeline layout!")
        }

        VkGraphicsPipelineCreateInfo pipeline_info {};
        pipeline_info.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount          = 2;
        pipeline_info.pStages             = shaderStages;
        pipeline_info.pVertexInputState   = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly;
        pipeline_info.pViewportState      = &viewport_state;
        pipeline_info.pRasterizationState = &rasterizer;
        pipeline_info.pMultisampleState   = &multisampling;
        pipeline_info.pColorBlendState    = &color_blending;
        pipeline_info.pDynamicState       = &dynamic_state;
        pipeline_info.layout              = m_pipeline_layout;
        pipeline_info.renderPass          = m_render_pass;
        pipeline_info.subpass             = 0;
        pipeline_info.basePipelineHandle  = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(m_device->m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_pipeline) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create graphics pipeline!");
        }

        m_shader->clear();
    }

    void VulkanPipeline::clear()
    {
        vkDestroyPipeline(m_device->m_device, m_pipeline, nullptr);
        vkDestroyPipelineLayout(m_device->m_device, m_pipeline_layout, nullptr);
    }
} // namespace ArchViz