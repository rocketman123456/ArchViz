#pragma once
#include "runtime/core/base/hash.h"
#include "runtime/core/base/macro.h"

#include "runtime/resource/resource_manager/compiler/compiler.h"
#include "runtime/resource/resource_manager/loader/loader.h"

#include "runtime/function/render/rhi/gpu_resource_handle.h"
#include "runtime/resource/resource_manager/resource_array.h"

#include <memory>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>

namespace ArchViz
{
    class GpuResourceManager
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

        template<typename T, typename C>
        void registerResourceCompiler();

        template<typename T, typename CI>
        ResourceHandle loadResource(const std::string& uri);

        template<typename T, typename CI>
        ResourceHandle loadResource(const std::string& uri, const CI& create_info);

        template<typename T, typename CI>
        ResourceHandle compileResource(const std::string& uri);

        template<typename T, typename CI>
        ResourceHandle compileResource(const std::string& uri, const CI& create_info);

        template<typename T>
        std::weak_ptr<T> getResource(const ResourceHandle& handle);

        template<typename T>
        std::weak_ptr<T> getResource(const std::string& uri);

    private:
        template<typename T>
        ResourceHandle createHandle();

        template<typename T>
        void addResource(const ResourceHandle& handle, std::shared_ptr<T> res, size_t size);

        template<typename T>
        void removeResource(const ResourceHandle& handle);

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

        std::unordered_map<std::string, ResourceHandle> m_resource_handles;     // uri -> handle
        std::unordered_map<ResourceHandle, std::string> m_resource_handles_inv; // uri -> handle

        std::unordered_map<const char*, std::shared_ptr<ILoader>>   m_resource_loaders;   // typeid -> loader
        std::unordered_map<const char*, std::shared_ptr<ICompiler>> m_resource_compilers; // typeid -> compiler
    };
} // namespace ArchViz
