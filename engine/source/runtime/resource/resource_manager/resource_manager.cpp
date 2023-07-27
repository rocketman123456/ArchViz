#include "runtime/resource/resource_manager/resource_manager.h"

#include "runtime/core/base/hash.h"

namespace ArchViz
{
    void ResourceManager::initialize()
    {
        m_obj_loader     = std::make_shared<ObjLoader>();
        m_texture_loader = std::make_shared<TextureLoader>();
    }

    void ResourceManager::clear()
    {
        m_obj_loader.reset();
        m_texture_loader.reset();
    }

    void ResourceManager::createMesh(const MeshComponentRes& res)
    {
        for (auto& sub_mesh : res.m_sub_meshes)
        {

        }
    }

    void ResourceManager::createMaterial(const MaterialRes& res)
    {
        res.m_base_colour_texture_file;
        res.m_metallic_roughness_texture_file;
        res.m_normal_texture_file;
        res.m_occlusion_texture_file;
        res.m_emissive_texture_file;
    }

    std::weak_ptr<MeshData> ResourceManager::getMeshData(const std::string& uri)
    {
        auto it = m_mesh_data_cache.find(uri);
        if (it == m_mesh_data_cache.end())
            return {};
        return it->second;
    }

    std::weak_ptr<TextureData> ResourceManager::getTextureData(const std::string& uri)
    {
        auto it = m_texture_cache.find(uri);
        if (it == m_texture_cache.end())
            return {};
        return it->second;
    }

} // namespace ArchViz
