#pragma once

#include "runtime/core/meta/reflection/reflection.h"

#include <volk.h>

#include <memory>
#include <string>

namespace ArchViz
{
    class AssetManager;
    class ConfigManager;
    class VulkanDevice;

    REFLECTION_TYPE(ShaderModuleConfig)
    CLASS(ShaderModuleConfig, Fields)
    {
    public:
        std::string m_vert_shader;
        std::string m_frag_shader;
        std::string m_geom_shader;
        std::string m_comp_shader;
        std::string m_tesc_shader;
        std::string m_tese_shader;
    };

    class VulkanShader
    {
    public:
        VulkanShader(const ShaderModuleConfig& config);
        ~VulkanShader();

        void initialize();
        void clear();

    private:
        void createShaderModule(const std::string file, VkShaderModule& shader);
        void destroyShaderModule(VkShaderModule& shader);

    public:
        std::shared_ptr<AssetManager>  m_asset_manager;
        std::shared_ptr<ConfigManager> m_config_manager;

        std::shared_ptr<VulkanDevice> m_device;

        ShaderModuleConfig m_config;

        VkShaderModule m_vert_shader = VK_NULL_HANDLE;
        VkShaderModule m_frag_shader = VK_NULL_HANDLE;
        VkShaderModule m_geom_shader = VK_NULL_HANDLE;
        VkShaderModule m_comp_shader = VK_NULL_HANDLE;
        VkShaderModule m_tesc_shader = VK_NULL_HANDLE;
        VkShaderModule m_tese_shader = VK_NULL_HANDLE;
    };
} // namespace ArchViz
