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

        virtual void resuorceDestroyed(const ResHandle& res) = 0;
    };

    template<typename T>
    class ResourceArray : public IResourceArray
    {
    public:
        void insertData(const ResHandle& handle, std::shared_ptr<T> resource)
        {
            ASSERT(m_handle_to_index.count(handle) == 0 && "Component added to same entity more than once.");
            // Put new entry at end
            size_t new_index             = m_size;
            m_handle_to_index[handle]    = new_index;
            m_index_to_handle[new_index] = handle;
            m_resources[new_index]       = resource;
            ++m_size;
        }

        T& getData(const ResHandle& handle)
        {
            ASSERT(m_handle_to_index.count(handle) != 0 && "Retrieving non-existent component.");
            return m_resources[m_handle_to_index[handle]];
        }

        void removeData(const ResHandle& handle);
        void entityDestroyed(const ResHandle& handle) override;

    private:
        std::array<std::shared_ptr<T>, k_max_resource_count> m_resources {};

        std::unordered_map<ResHandle, size_t> m_handle_to_index {};
        std::unordered_map<size_t, ResHandle> m_index_to_handle {};
        size_t                                m_size {};
    };
} // namespace ArchViz
