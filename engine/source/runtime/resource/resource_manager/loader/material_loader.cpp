#include "runtime/resource/resource_manager/loader/material_loader.h"

#include "runtime/function/global/global_context.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"
#include "runtime/resource/resource_manager/resource_manager.h"

namespace ArchViz
{
    std::pair<std::shared_ptr<MaterialData>, size_t> MaterialLoader::createResource(const MaterialRes& create_info)
    {
        std::shared_ptr<MaterialData> material = std::make_shared<MaterialData>();

        if (!create_info.m_base_colour_texture_file.empty())
        {
            material->m_base_colour = g_runtime_global_context.m_resource_manager->loadResource<TextureData, TextureRes>(create_info.m_base_colour_texture_file);
        }
        if (!create_info.m_metallic_roughness_texture_file.empty())
        {
            material->m_metallic_roughness = g_runtime_global_context.m_resource_manager->loadResource<TextureData, TextureRes>(create_info.m_metallic_roughness_texture_file);
        }
        if (!create_info.m_normal_texture_file.empty())
        {
            material->m_normal = g_runtime_global_context.m_resource_manager->loadResource<TextureData, TextureRes>(create_info.m_normal_texture_file);
        }
        if (!create_info.m_occlusion_texture_file.empty())
        {
            material->m_occlusion = g_runtime_global_context.m_resource_manager->loadResource<TextureData, TextureRes>(create_info.m_occlusion_texture_file);
        }
        if (!create_info.m_emissive_texture_file.empty())
        {
            material->m_emissive = g_runtime_global_context.m_resource_manager->loadResource<TextureData, TextureRes>(create_info.m_emissive_texture_file);
        }

        return {material, sizeof(MaterialData)};
    }

    std::pair<std::shared_ptr<MaterialData>, size_t> MaterialLoader::createResource(const std::string& uri)
    {
        MaterialRes res;
        g_runtime_global_context.m_asset_manager->loadAsset(uri, res);

        return createResource(res);
    }
} // namespace ArchViz
