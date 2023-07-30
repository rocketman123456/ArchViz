#include "runtime/resource/resource_manager/resource_manager.h"

#include "runtime/resource/resource_manager/loader/obj_loader.h"
#include "runtime/resource/resource_manager/loader/texture_loader.h"

#include "runtime/resource/res_type/data/material_data.h"
#include "runtime/resource/res_type/data/mesh_data.h"

#include "runtime/core/base/hash.h"
#include "runtime/core/base/macro.h"

namespace ArchViz
{
    void ResourceManager::initialize()
    {
        registerResourceType<MeshData>();
        registerResourceType<TextureData>();

        registerResourceLoader<MeshData, ObjLoader>();
        registerResourceLoader<MeshData, TextureLoader>();
    }

    void ResourceManager::clear()
    {
        m_resource_types.clear();
        m_resource_arrays.clear();
        m_loaders.clear();
    }

} // namespace ArchViz
