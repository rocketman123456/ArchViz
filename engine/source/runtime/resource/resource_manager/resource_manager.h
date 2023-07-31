// https://floooh.github.io/2018/06/17/handles-vs-pointers.html
// https://giordi91.github.io/post/resourcesystem/
#pragma once
#include "runtime/core/base/hash.h"
#include "runtime/core/base/macro.h"

#include "runtime/resource/resource_manager/loader/loader.h"

#include "runtime/resource/resource_manager/resource_array.h"
#include "runtime/resource/resource_manager/resource_handle.h"

#include <memory>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>

namespace ArchViz
{
    struct ResourceManagerCreateInfo
    {
        std::unordered_map<const char*, size_t> resource_max_counts;
        std::unordered_map<const char*, size_t> resource_max_sizes;
    };

    // a handle based simple resource manager
    class ResourceManager
    {
    public:
        void initialize();
        void clear();

        template<typename T>
        void registerResourceType();

        template<typename T>
        ResourceTypeId getResourceType() const;

        const char* getResourceTypeName(ResourceTypeId type) const;

        template<typename T, typename L>
        void registerResourceLoader();

        template<typename T, typename CI>
        ResHandle loadResource(const std::string& uri);

        template<typename T, typename CI>
        ResHandle loadResource(const std::string& uri, const CI& create_info);

        template<typename T>
        std::weak_ptr<T> getResource(const ResHandle& handle);

        template<typename T>
        std::weak_ptr<T> getResource(const std::string& uri);

    private:
        template<typename T>
        ResHandle createHandle();

        template<typename T>
        void addResource(const ResHandle& handle, std::shared_ptr<T> res, size_t size);

        template<typename T>
        void removeResource(const ResHandle& handle);

        template<typename T>
        void removeResource(const std::string& uri);

        template<typename T>
        std::shared_ptr<ResourceArray<T>> getResourceArray();

        template<typename T>
        std::shared_ptr<ResourceArray<T>> getResourceArray(ResourceTypeId type);

    private:
        std::unordered_map<const char*, size_t> m_resource_max_counts; // software constraints
        std::unordered_map<const char*, size_t> m_resource_max_sizes;  // hardware constraints

        std::unordered_map<const char*, ResourceTypeId> m_resource_types;     // resource type map
        std::unordered_map<ResourceTypeId, const char*> m_resource_types_inv; // resource type map

        std::unordered_map<const char*, std::shared_ptr<IResourceArray>>    m_resource_arrays;    // resource storage
        std::unordered_map<ResourceTypeId, std::shared_ptr<IResourceArray>> m_resource_id_arrays; // resource storage

        std::unordered_map<std::string, ResHandle> m_resource_handles;     // uri -> handle
        std::unordered_map<ResHandle, std::string> m_resource_handles_inv; // uri -> handle

        std::unordered_map<const char*, std::shared_ptr<ILoader>> m_resource_loaders; // typeid -> loader
    };

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
    void ResourceManager::addResource(const ResHandle& handle, std::shared_ptr<T> res, size_t size)
    {
        // Add a component to the array for an handle
        getResourceArray<T>(handle.type)->insertData(handle.index, res, size);
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

    template<typename T>
    std::weak_ptr<T> ResourceManager::getResource(const std::string& uri)
    {
        if (m_resource_handles.count(uri) == 0)
        {
            LOG_WARN("cannot find resource: {}", uri);
        }
        auto& handle = m_resource_handles[uri];
        // Get a reference to a component from the array for an handle
        return getResourceArray<T>(handle.type)->getData(handle.index);
    }

    template<typename T, typename CI>
    ResHandle ResourceManager::loadResource(const std::string& uri)
    {
        if (m_resource_handles.count(uri) != 0)
        {
            LOG_DEBUG("resource loaded: {}", uri);
            return m_resource_handles[uri];
        }

        const std::type_info& type = typeid(T);
        if (m_resource_loaders.count(type.name()) == 0)
        {
            LOG_ERROR("cannot find a valid loadr for: {}", type.name());
            return k_invalid_res_handle;
        }

        LOG_DEBUG("load resource: {}", uri);

        std::shared_ptr<Loader<T, CI>> loader = std::static_pointer_cast<Loader<T, CI>>(m_resource_loaders[type.name()]);

        std::shared_ptr<T> res;
        size_t             size;
        std::tie(res, size) = loader->createResource(uri);
        if (res != nullptr)
        {
            ResHandle handle               = createHandle<T>();
            m_resource_handles[uri]        = handle;
            m_resource_handles_inv[handle] = uri;
            addResource<T>(handle, res, size);
            return handle;
        }
        return k_invalid_res_handle;
    }

    template<typename T, typename CI>
    ResHandle ResourceManager::loadResource(const std::string& uri, const CI& create_info)
    {
        if (m_resource_handles.count(uri) != 0)
        {
            LOG_DEBUG("resource loaded: {}", uri);
            return m_resource_handles[uri];
        }

        const std::type_info& type = typeid(T);
        if (m_resource_loaders.count(type.name()) == 0)
        {
            LOG_ERROR("cannot find a valid loadr for: {}", type.name());
            return k_invalid_res_handle;
        }

        LOG_DEBUG("load resource: {}", uri);

        std::shared_ptr<Loader<T, CI>> loader = std::static_pointer_cast<Loader<T, CI>>(m_resource_loaders[type.name()]);

        std::shared_ptr<T> res;
        size_t             size;
        std::tie(res, size) = loader->createResource(create_info);
        if (res != nullptr)
        {
            ResHandle handle               = createHandle<T>();
            m_resource_handles[uri]        = handle;
            m_resource_handles_inv[handle] = uri;
            addResource<T>(handle, res, size);
            return handle;
        }

        return k_invalid_res_handle;
    }

    template<typename T>
    std::shared_ptr<ResourceArray<T>> ResourceManager::getResourceArray()
    {
        const char* type_name = typeid(T).name();
        ASSERT(m_resource_types.count(type_name) != 0 && "Component not registered before use.");
        return std::static_pointer_cast<ResourceArray<T>>(m_resource_arrays[type_name]);
    }

    template<typename T>
    std::shared_ptr<ResourceArray<T>> ResourceManager::getResourceArray(ResourceTypeId type)
    {
        ASSERT(m_resource_id_arrays.count(type) != 0 && "Component not registered before use.");
        return std::static_pointer_cast<ResourceArray<T>>(m_resource_id_arrays[type]);
    }
} // namespace ArchViz
