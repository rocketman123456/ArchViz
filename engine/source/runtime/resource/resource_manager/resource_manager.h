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
    class ResourceManager // : public std::enable_shared_from_this<ResourceManager>
    {
    public:
        void initialize();
        void clear();

        template<typename T>
        void registerResourceType();

        template<typename T>
        ResourceTypeId getResourceType();

        template<typename T, typename L>
        void registerResourceLoader();

        template<typename T, typename CI>
        std::weak_ptr<T> loadResource(const std::string& uri)
        {
            const std::type_info& type = typeid(T);
            if (m_loaders.count(type.name()) != 0)
            {
                std::shared_ptr<Loader<T, CI>> loader = std::static_pointer_cast<Loader<T, CI>>(m_loaders[type.name()]);
                std::shared_ptr<T>             res    = loader.createResource(uri);
                if (res != nullptr)
                {
                    //
                }
            }
            return {};
        }

        template<typename T, typename CI>
        std::weak_ptr<T> loadResource(const std::string& uri, const CI& create_info)
        {
            if (m_resource_handles.count(uri) != 0)
            {
                // return existed resources
                ResHandle&       handle = m_resource_handles.find(uri)->second;
                std::weak_ptr<T> res    = getResource<T>(handle);
                return res;
            }

            const std::type_info& type = typeid(T);
            if (m_loaders.count(type.name()) != 0)
            {
                std::shared_ptr<Loader<T, CI>> loader = std::static_pointer_cast<Loader<T, CI>>(m_loaders[type.name()]);

                std::shared_ptr<T> res;
                size_t             size;
                std::tie(res, size) = loader.createResource(create_info);
                if (res != nullptr)
                {
                    auto& array = m_resource_arrays[type.name()];
                    auto& type  = m_resource_types[type.name()];

                    ResHandle handle;
                    handle.type  = type;
                    handle.index = ResourceIdAllocator::alloc();

                    // add resource to array
                    addResource(handle, res);
                }
            }
            return {};
        }

        template<typename T>
        void addResource(const ResHandle& handle, std::shared_ptr<T> res)
        {
            // Add a component to the array for an handle
            getResourceArray<T>()->insertData(handle, res);
        }

        template<typename T>
        void removeResource(const ResHandle& handle)
        {
            // Remove a component from the array for an handle
            getResourceArray<T>()->removeData(handle);
        }

        template<typename T>
        std::shared_ptr<T> getResource(const ResHandle& handle)
        {
            // Get a reference to a component from the array for an handle
            return getResourceArray<T>()->getData(handle);
        }

        template<typename T>
        void unloadResource(const std::string& uri)
        {
            // find resource data pointer
            // delete resource and update handle
        }

    private:
        template<typename T>
        std::shared_ptr<ResourceArray<T>> getResourceArray()
        {
            const char* type_name = typeid(T).name();

            ASSERT(m_resource_types.count(type_name) != 0 && "Component not registered before use.");

            return std::static_pointer_cast<ResourceArray<T>>(m_resource_arrays[type_name]);
        }

    private:
        std::unordered_map<const char*, size_t> m_resource_max_counts;
        std::unordered_map<const char*, size_t> m_resource_max_sizes;

        std::unordered_map<const char*, ResourceTypeId>                  m_resource_types;
        std::unordered_map<const char*, std::shared_ptr<IResourceArray>> m_resource_arrays;
        std::unordered_map<std::string, ResHandle>                       m_resource_handles;
        std::unordered_map<const char*, std::shared_ptr<ILoader>>        m_loaders;
    };
} // namespace ArchViz
