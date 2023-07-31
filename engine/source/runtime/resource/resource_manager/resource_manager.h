// https://floooh.github.io/2018/06/17/handles-vs-pointers.html
// https://giordi91.github.io/post/resourcesystem/
#pragma once
#include "runtime/core/base/instanceof.h"
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
        const char*    getResourceTypeName(ResourceTypeId type) const;

        template<typename T, typename L>
        void registerResourceLoader();

        template<typename T>
        ResHandle createHandle();

        template<typename T, typename CI>
        std::weak_ptr<T> loadResource(const std::string& uri);

        template<typename T, typename CI>
        std::weak_ptr<T> loadResource(const std::string& uri, const CI& create_info);

        template<typename T>
        void addResource(const ResHandle& handle, std::shared_ptr<T> res);

        template<typename T>
        void removeResource(const ResHandle& handle);

        template<typename T>
        void removeResource(const std::string& uri);

        template<typename T>
        std::weak_ptr<T> getResource(const ResHandle& handle);

    private:
        template<typename T>
        std::shared_ptr<ResourceArray<T>> getResourceArray()
        {
            const char* type_name = typeid(T).name();
            ASSERT(m_resource_types.count(type_name) != 0 && "Component not registered before use.");
            return std::static_pointer_cast<ResourceArray<T>>(m_resource_arrays[type_name]);
        }

        template<typename T>
        std::shared_ptr<ResourceArray<T>> getResourceArray(ResourceTypeId type)
        {
            ASSERT(m_resource_id_arrays.count(type) != 0 && "Component not registered before use.");
            return std::static_pointer_cast<ResourceArray<T>>(m_resource_id_arrays[type]);
        }

    private:
        std::unordered_map<const char*, size_t>                             m_resource_max_counts;  // software constraints
        std::unordered_map<const char*, size_t>                             m_resource_max_sizes;   // hardware constraints
        std::unordered_map<const char*, ResourceTypeId>                     m_resource_types;       // resource type map
        std::unordered_map<ResourceTypeId, const char*>                     m_resource_types_inv;   // resource type map
        std::unordered_map<const char*, std::shared_ptr<IResourceArray>>    m_resource_arrays;      // resource storage
        std::unordered_map<ResourceTypeId, std::shared_ptr<IResourceArray>> m_resource_id_arrays;   // resource storage
        std::unordered_map<const char*, std::shared_ptr<ILoader>>           m_resource_loaders;     // typeid -> loader
        std::unordered_map<std::string, ResHandle>                          m_resource_handles;     // uri -> handle
        std::unordered_map<ResHandle, std::string>                          m_resource_handles_inv; // uri -> handle
    };
} // namespace ArchViz
