#include "runtime/resource/resource_manager/resource_array.h"

namespace ArchViz
{
    template<typename T>
    void ResourceArray<T>::removeData(const ResHandle& handle)
    {
        ASSERT(m_handle_to_index.find(handle) != 0 && "Removing non-existent component.");

        // Copy element at end into deleted element's place to maintain density
        size_t index_of_removed_entity        = m_handle_to_index[handle];
        size_t index_of_last_element          = m_size - 1;
        m_resources[index_of_removed_entity]  = m_resources[index_of_last_element];

        // Update map to point to moved spot
        ResHandle& handle_of_last_element          = m_index_to_handle[index_of_last_element];
        m_handle_to_index[index_of_last_element]   = index_of_removed_entity;
        m_index_to_handle[index_of_removed_entity] = handle_of_last_element;

        m_resources[index_of_last_element] = nullptr;

        m_handle_to_index.erase(handle);
        m_index_to_handle.erase(index_of_last_element);
        --m_size;
    }

    template<typename T>
    void ResourceArray<T>::entityDestroyed(const ResHandle& handle)
    {
        if (m_handle_to_index.count(handle) != 0)
        {
            removeData(handle);
        }
    }
} // namespace ArchViz
