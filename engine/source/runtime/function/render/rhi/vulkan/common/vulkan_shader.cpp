#include "runtime/function/render/rhi/vulkan/common/vulkan_shader.h"
#include "runtime/function/render/rhi/vulkan/common/vulkan_device.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_shader_utils.h"

#include "runtime/function/global/global_context.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

namespace ArchViz
{
    VulkanShader::VulkanShader(const ShaderModuleCreateInfo& config) : m_config(config) {}

    void VulkanShader::initialize()
    {
        createShaderModule(m_config.m_vert_shader, m_vert_shader);
        createShaderModule(m_config.m_frag_shader, m_frag_shader);
        createShaderModule(m_config.m_geom_shader, m_geom_shader);
        createShaderModule(m_config.m_comp_shader, m_comp_shader);
        createShaderModule(m_config.m_tesc_shader, m_tesc_shader);
        createShaderModule(m_config.m_tese_shader, m_tese_shader);

        if (m_vert_shader != VK_NULL_HANDLE)
        {
            auto info = getStageInfo(m_vert_shader, VK_SHADER_STAGE_VERTEX_BIT);
            m_stage_info.push_back(info);
        }
        if (m_frag_shader != VK_NULL_HANDLE)
        {
            auto info = getStageInfo(m_frag_shader, VK_SHADER_STAGE_FRAGMENT_BIT);
            m_stage_info.push_back(info);
        }
        if (m_geom_shader != VK_NULL_HANDLE)
        {
            auto info = getStageInfo(m_geom_shader, VK_SHADER_STAGE_GEOMETRY_BIT);
            m_stage_info.push_back(info);
        }
        if (m_comp_shader != VK_NULL_HANDLE)
        {
            auto info = getStageInfo(m_comp_shader, VK_SHADER_STAGE_COMPUTE_BIT);
            m_stage_info.push_back(info);
        }
        if (m_tesc_shader != VK_NULL_HANDLE)
        {
            auto info = getStageInfo(m_tesc_shader, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
            m_stage_info.push_back(info);
        }
        if (m_tese_shader != VK_NULL_HANDLE)
        {
            auto info = getStageInfo(m_tese_shader, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
            m_stage_info.push_back(info);
        }
    }

    void VulkanShader::clear()
    {
        destroyShaderModule(m_vert_shader);
        destroyShaderModule(m_frag_shader);
        destroyShaderModule(m_geom_shader);
        destroyShaderModule(m_comp_shader);
        destroyShaderModule(m_tesc_shader);
        destroyShaderModule(m_tese_shader);
    }

    const std::vector<VkPipelineShaderStageCreateInfo>& VulkanShader::getStageInfos() const { return m_stage_info; }

    VkPipelineShaderStageCreateInfo VulkanShader::getStageInfo(VkShaderModule module, VkShaderStageFlagBits stage)
    {
        VkPipelineShaderStageCreateInfo stage_info {};
        stage_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stage_info.stage  = stage;
        stage_info.module = module;
        stage_info.pName  = "main";
        return stage_info;
    }

    void VulkanShader::createShaderModule(const std::string file, VkShaderModule& shader)
    {
        if (file.size() > 0)
        {
            auto spv = VulkanShaderUtils::createShaderModuleFromVFS(file);
            shader   = VulkanShaderUtils::createShaderModule(m_device->m_device, spv);
        }
    }

    void VulkanShader::destroyShaderModule(VkShaderModule& shader)
    {
        if (shader != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(m_device->m_device, shader, nullptr);
            shader = VK_NULL_HANDLE;
        }
    }
} // namespace ArchViz
