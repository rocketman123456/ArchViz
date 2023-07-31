#include "runtime/resource/resource_manager/resource_manager.h"

#include "runtime/resource/resource_manager/loader/material_loader.h"
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

    template<typename T>
    void ResourceManager::registerResourceType()
    {
        const std::type_info& type = typeid(T);
        if (m_resource_types.count(type.name()) == 0)
        {
            // TODO : read from config file
            ResourceTypeId id              = ResourceTypeIdAllocator::alloc();
            m_resource_types[type.name()]  = id;
            m_resource_types_inv[id]       = type.name();
            m_resource_arrays[type.name()] = std::make_shared<ResourceArray<T>>(k_max_resource_count, k_max_resource_size);
            m_resource_id_arrays[id]       = m_resource_arrays[type.name()];
            // TODO : use this to trigger memory collect
            m_resource_max_counts[type.name()] = k_max_resource_count;
            m_resource_max_sizes[type.name()]  = k_max_resource_size;
        }
        else
        {
            LOG_WARN("Registering resource type: {} more than once.", type.name());
        }
    }

    template<typename T>
    ResourceTypeId ResourceManager::getResourceType() const
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

    template<typename T, typename L>
    void ResourceManager::registerResourceLoader()
    {
        // one resource type can only have one loader now
        const std::type_info& type = typeid(T);

        if (m_resource_loaders.count(type.name()) == 0)
        {
            std::shared_ptr<L> loader = std::make_shared<L>();

            // must be a ILoader class
            bool is_loader = std::is_base_of<ILoader, L>::value;
            ASSERT(is_loader && "not a valid loader class");

            m_resource_loaders[type.name()] = loader;
        }
    }

    template<typename T>
    ResHandle ResourceManager::createHandle()
    {
        const auto& type    = typeid(T);
        auto        type_id = m_resource_types[type.name()];
        auto        index   = ResourceIdAllocator::alloc();
        return {k_magic_id, type_id, index};
    }

    template<typename T>
    void ResourceManager::addResource(const ResHandle& handle, std::shared_ptr<T> res)
    {
        // Add a component to the array for an handle
        getResourceArray<T>(handle.type)->insertData(handle.index, res);
    }

    template<typename T>
    void ResourceManager::removeResource(const ResHandle& handle)
    {
        if (m_resource_handles_inv.count(handle) == 0)
        {
            LOG_WARN("remove non-exist resource handle: {}", handle.index);
            return;
        }

        // Remove a component from the array for an handle
        getResourceArray<T>(handle.type)->removeData(handle.index);

        std::string name = m_resource_handles_inv[handle];
        m_resource_handles_inv.erase(handle);
        m_resource_handles.erase(name);
    }

    template<typename T>
    void ResourceManager::removeResource(const std::string& uri)
    {
        if (m_resource_handles.count(uri) == 0)
        {
            LOG_WARN("remove non-exist resource: {}", uri);
            return;
        }

        auto handle = m_resource_handles[uri];

        // Remove a component from the array for an handle
        getResourceArray<T>(handle.type)->removeData(handle.index);

        m_resource_handles_inv.erase(handle);
        m_resource_handles.erase(uri);
    }

    template<typename T>
    std::weak_ptr<T> ResourceManager::getResource(const ResHandle& handle)
    {
        // Get a reference to a component from the array for an handle
        return getResourceArray<T>(handle.type)->getData(handle.index);
    }

    template<typename T, typename CI>
    std::weak_ptr<T> ResourceManager::loadResource(const std::string& uri)
    {
        if (m_resource_handles.count(uri) != 0)
        {
            return getResource<T>(m_resource_handles[uri]);
        }

        const std::type_info& type = typeid(T);
        if (m_resource_loaders.count(type.name()) == 0)
        {
            LOG_ERROR("cannot find a valid loadr for: {}", type.name());
            return {};
        }

        std::shared_ptr<Loader<T, CI>> loader = std::static_pointer_cast<Loader<T, CI>>(m_resource_loaders[type.name()]);
        std::shared_ptr<T>             res    = loader.createResource(uri);
        if (res != nullptr)
        {
            auto handle                    = createHandle<T>();
            m_resource_handles[uri]        = res;
            m_resource_handles_inv[handle] = res;
            addResource<T>(handle, res);
            return res;
        }
        return {};
    }

    template<typename T, typename CI>
    std::weak_ptr<T> ResourceManager::loadResource(const std::string& uri, const CI& create_info)
    {
        if (m_resource_handles.count(uri) != 0)
        {
            return getResource<T>(m_resource_handles[uri]);
        }

        const std::type_info& type = typeid(T);
        if (m_resource_loaders.count(type.name()) == 0)
        {
            LOG_ERROR("cannot find a valid loadr for: {}", type.name());
            return {};
        }

        std::shared_ptr<Loader<T, CI>> loader = std::static_pointer_cast<Loader<T, CI>>(m_resource_loaders[type.name()]);
        std::shared_ptr<T>             res    = loader.createResource(create_info);
        if (res != nullptr)
        {
            auto handle                    = createHandle<T>();
            m_resource_handles[uri]        = res;
            m_resource_handles_inv[handle] = res;
            addResource<T>(handle, res);
            return res;
        }

        return {};
    }

} // namespace ArchViz
