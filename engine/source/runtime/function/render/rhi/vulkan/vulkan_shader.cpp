#include "runtime/function/render/rhi/vulkan/vulkan_shader.h"
#include "runtime/function/render/rhi/vulkan/vulkan_device.h"
#include "runtime/function/render/rhi/vulkan/vulkan_shader_utils.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

namespace ArchViz
{
    VulkanShader::VulkanShader(const ShaderModuleConfig& config) : m_config(config) {}

    VulkanShader::~VulkanShader() { clear(); }

    void VulkanShader::initialize()
    {
        createShaderModule(m_config.m_vert_shader, m_vert_shader);
        createShaderModule(m_config.m_frag_shader, m_frag_shader);
        createShaderModule(m_config.m_geom_shader, m_geom_shader);
        createShaderModule(m_config.m_comp_shader, m_comp_shader);
        createShaderModule(m_config.m_tesc_shader, m_tesc_shader);
        createShaderModule(m_config.m_tese_shader, m_tese_shader);
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

    void VulkanShader::createShaderModule(const std::string file, VkShaderModule& shader)
    {
        if (file.size() > 0)
        {
            std::vector<uint32_t> spv = VulkanShaderUtils::createShaderModuleFromFile(file, m_config_manager, m_asset_manager);
            shader                    = VulkanShaderUtils::createShaderModule(m_device->m_logical_device, spv);
        }
    }

    void VulkanShader::destroyShaderModule(VkShaderModule& shader)
    {
        if (shader != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(m_device->m_logical_device, shader, nullptr);
            shader = VK_NULL_HANDLE;
        }
    }
} // namespace ArchViz
