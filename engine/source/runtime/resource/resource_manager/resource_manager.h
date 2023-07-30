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
    class ResourceManager : public std::enable_shared_from_this<ResourceManager>
    {
    public:
        void initialize();
        void clear();

        template<typename T>
        void registerResourceType()
        {
            const std::type_info& type = typeid(T);
            if (m_resource_types.count(type.name()) == 0)
            {
                ResourceTypeId id             = ResourceTypeIdAllocator::alloc();
                m_resource_types[type.name()] = id;
            }
            else
            {
                LOG_WARN("Registering resource type: {} more than once.", type.name());
            }
        }

        template<typename T>
        ResourceTypeId getResourceType()
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
        void registerResourceLoader()
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
        std::weak_ptr<T> loadResource(const CI& create_info)
        {
            const std::type_info& type = typeid(T);
            if (m_loaders.count(type.name()) != 0)
            {
                std::shared_ptr<Loader<T, CI>> loader = std::static_pointer_cast<Loader<T, CI>>(m_loaders[type.name()]);
                std::shared_ptr<T>             res    = loader.createResource(create_info);
                if (res != nullptr)
                {
                    //
                }
            }
            return {};
        }

        template<typename T>
        void unloadResource(const std::string& uri)
        {
            //
        }

    private:
        template<typename T>
        std::shared_ptr<ResourceArray<T>> GetResourceArray()
        {
            const char* type_name = typeid(T).name();

            ASSERT(m_resource_types.count(type_name) != 0 && "Component not registered before use.");

            return std::static_pointer_cast<ResourceArray<T>>(m_resource_arrays[type_name]);
        }

    private:
        std::unordered_map<const char*, ResourceTypeId>                  m_resource_types;
        std::unordered_map<const char*, std::shared_ptr<IResourceArray>> m_resource_arrays;
        std::unordered_map<const char*, std::shared_ptr<ILoader>>        m_loaders;
    };
} // namespace ArchViz
