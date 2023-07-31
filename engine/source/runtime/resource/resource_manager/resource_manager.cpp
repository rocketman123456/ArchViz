#include "runtime/resource/resource_manager/resource_manager.h"

#include "runtime/resource/resource_manager/loader/material_loader.h"
#include "runtime/resource/resource_manager/loader/obj_loader.h"
#include "runtime/resource/resource_manager/loader/texture_loader.h"

#include "runtime/resource/res_type/data/material_data.h"
#include "runtime/resource/res_type/data/mesh_data.h"

namespace ArchViz
{
    void ResourceManager::initialize()
    {
        registerResourceType<MeshData>();
        registerResourceType<TextureData>();
        registerResourceType<MaterialData>();

        registerResourceLoader<MeshData, ObjLoader>();
        registerResourceLoader<TextureData, TextureLoader>();
        registerResourceLoader<MaterialData, MaterialLoader>();
    }

    void ResourceManager::clear()
    {
        m_resource_max_counts.clear();
        m_resource_max_sizes.clear();
        m_resource_types.clear();
        m_resource_types_inv.clear();
        m_resource_arrays.clear();
        m_resource_id_arrays.clear();
        m_resource_loaders.clear();
        m_resource_handles.clear();
    }

    const char* ResourceManager::getResourceTypeName(ResourceTypeId type) const
    {
        static const char* invalid_name = "invalid_type_id";
        if (m_resource_types_inv.count(type) == 0)
        {
            LOG_ERROR("try to get invalid type id's name: {}", type);
            return invalid_name;
        }
        else
        {
            // cannot use [] operator here
            auto it = m_resource_types_inv.find(type);
            return it->second;
        }
    }
} // namespace ArchViz
