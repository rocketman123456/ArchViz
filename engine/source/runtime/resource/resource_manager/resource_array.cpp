#include "runtime/resource/resource_manager/resource_array.h"

namespace ArchViz
{
    template<typename T>
    ResourceArray<T>::ResourceArray(size_t count, size_t size) : m_max_count(count), m_max_size(size)
    {
        m_resources.resize(m_max_count);
        m_resource_sizes.resize(m_max_count);
    }

    template<typename T>
    void ResourceArray<T>::insertData(const ResourceId& handle, std::shared_ptr<T> resource, size_t size)
    {
        ASSERT(m_handle_to_index.count(handle) == 0 && "Component added to same entity more than once.");
        if (m_content_size >= m_max_size)
        {
            LOG_WARN("Resource Array size exceed max content size: {}", m_max_size);
        }
        if (m_array_size >= m_max_size)
        {
            LOG_ERROR("Resource Array size exceed max array size: {}", m_array_size);
        }
        // Put new entry at end
        size_t new_index             = m_array_size;
        m_handle_to_index[handle]    = new_index;
        m_index_to_handle[new_index] = handle;
        m_resources[new_index]       = resource;
        m_resource_sizes[new_index]  = size;
        m_content_size += size;
        m_array_size += 1;
    }

    template<typename T>
    std::weak_ptr<T> ResourceArray<T>::getData(const ResourceId& handle)
    {
        // we can get non-existent resource with nullptr
        // ASSERT(m_handle_to_index.count(handle) != 0 && "Retrieving non-existent resource.");
        return m_resources[m_handle_to_index[handle]];
    }

    template<typename T>
    void ResourceArray<T>::removeData(const ResourceId& handle)
    {
        ASSERT(m_handle_to_index.count(handle) != 0 && "Removing non-existent resource.");

        // Copy element at end into deleted element's place to maintain density
        // with smart pointer to avoid real memory copy
        size_t index_of_removed_handle       = m_handle_to_index[handle];
        size_t index_of_last_element         = m_array_size - 1;
        m_resources[index_of_removed_handle] = m_resources[index_of_last_element];
        m_resources[index_of_last_element]   = nullptr;

        // Update map to point to moved spot
        ResHandle& handle_of_last_element          = m_index_to_handle[index_of_last_element];
        m_handle_to_index[index_of_last_element]   = index_of_removed_handle;
        m_index_to_handle[index_of_removed_handle] = handle_of_last_element;

        // update size record
        m_content_size -= m_resource_sizes[index_of_removed_handle];
        m_resource_sizes[index_of_removed_handle] = m_resource_sizes[index_of_last_element];
        m_resource_sizes[index_of_last_element]   = 0;

        // remove unused record
        m_handle_to_index.erase(handle);
        m_index_to_handle.erase(index_of_last_element);
        --m_array_size;
    }

    template<typename T>
    void ResourceArray<T>::handleDestroyed(const ResourceId& handle)
    {
        if (m_handle_to_index.count(handle) != 0)
        {
            removeData(handle);
        }
    }
} // namespace ArchViz
