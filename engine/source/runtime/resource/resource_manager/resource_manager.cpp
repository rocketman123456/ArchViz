#include "runtime/resource/resource_manager/resource_manager.h"

#include "runtime/resource/resource_manager/loader/obj_loader.h"
#include "runtime/resource/resource_manager/loader/texture_loader.h"
#include "runtime/resource/resource_manager/loader/material_loader.h"

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
        registerResourceType<MaterialData>();

        registerResourceLoader<MeshData, ObjLoader>();
        registerResourceLoader<TextureData, TextureLoader>();
        registerResourceLoader<MaterialData, MaterialLoader>();
    }

    void ResourceManager::clear()
    {
        m_resource_types.clear();
        m_resource_arrays.clear();
        m_loaders.clear();
    }

    template<typename T>
    void ResourceManager::registerResourceType()
    {
        const std::type_info& type = typeid(T);
        if (m_resource_types.count(type.name()) == 0)
        {
            // TODO : read from config file
            ResourceTypeId id              = ResourceTypeIdAllocator::alloc();
            m_resource_types[type.name()]  = id;
            m_resource_arrays[type.name()] = std::make_shared<ResourceArray<T>>(k_max_resource_count, k_max_resource_size);
        }
        else
        {
            LOG_WARN("Registering resource type: {} more than once.", type.name());
        }
    }

    template<typename T>
    ResourceTypeId ResourceManager::getResourceType()
    {
        const std::type_info& type = typeid(T);
        if (m_resource_types.count(type.name()) != 0)
        {
            const std::type_info& type = typeid(T);
            return m_resource_types[type.name()];
        }
        else
        {
            return k_invalid_resource_type_id;
        }
    }

    template<typename T, typename L>
    void ResourceManager::registerResourceLoader()
    {
        const std::type_info& type = typeid(T);

        if (m_loaders.count(type.name()) == 0)
        {
            std::shared_ptr<L> loader = std::make_shared<L>();

            bool is_loader = std::is_base_of<ILoader, L>::value;
            ASSERT(is_loader && "not a valid loader class");

            m_loaders[type.name()] = loader;
        }
    }

} // namespace ArchViz
