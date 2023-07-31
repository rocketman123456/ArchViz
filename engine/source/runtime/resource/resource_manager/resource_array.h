#pragma once
#include "runtime/core/base/macro.h"
#include "runtime/resource/resource_manager/resource_handle.h"

#include <array>
#include <atomic>
#include <cassert>
#include <memory>
#include <unordered_map>

namespace ArchViz
{
    // The virtual inheritance of IComponentArray is unfortunate but, as far as I can tell, unavoidable. As seen later, we'll have a list of every ComponentArray (one per component type), and we need
    // to notify all of them when an entity is destroyed so that it can remove the entity's data if it exists. The only way to keep a list of multiple templated types is to keep a list of their common
    // interface so that we can call EntityDestroyed() on all of them.
    class IResourceArray
    {
    public:
        virtual ~IResourceArray() = default;

        virtual void handleDestroyed(const ResourceId& res) = 0;
    };

    template<typename T>
    class ResourceArray : public IResourceArray
    {
    public:
        explicit ResourceArray(size_t count, size_t size);
        virtual ~ResourceArray() = default;

        std::weak_ptr<T> getData(const ResourceId& handle);

        void insertData(const ResourceId& handle, std::shared_ptr<T> resource, size_t size);
        void removeData(const ResourceId& handle);
        void handleDestroyed(const ResourceId& handle) override;

    private:
        size_t m_max_count;
        size_t m_max_size;

        std::vector<std::shared_ptr<T>> m_resources {};
        std::vector<size_t>             m_resource_sizes {};

        std::unordered_map<ResourceId, size_t> m_handle_to_index {};
        std::unordered_map<size_t, ResourceId> m_index_to_handle {};
        size_t                                 m_array_size {0};
        size_t                                 m_content_size {0};
    };
} // namespace ArchViz
