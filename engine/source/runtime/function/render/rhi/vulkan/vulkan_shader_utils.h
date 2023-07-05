#pragma once
#include "runtime/function/render/rhi/rhi.h"

#include <volk.h>

#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace ArchViz
{
    class ConfigManager;
    class AssetManager;

    class VulkanShaderUtils
    {
    public:
        static std::vector<uint32_t> createShaderModuleFromVFS(const std::string& shader_file, std::shared_ptr<ConfigManager> config_manager, std::shared_ptr<AssetManager> asset_manager);
        static std::vector<uint32_t> createShaderModuleFromFile(const std::string& shader_file, std::shared_ptr<ConfigManager> config_manager, std::shared_ptr<AssetManager> asset_manager);
        static std::vector<uint32_t> createShaderModuleFromCode(const std::string& shader_code, const std::string& shader_type, std::shared_ptr<ConfigManager> config_manager);
        static VkShaderModule        createShaderModule(VkDevice device, const std::vector<uint32_t>& shader_code);
    };
} // namespace ArchViz
