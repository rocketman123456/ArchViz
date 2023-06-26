#include "runtime/function/framework/ecs/component_array.h"

namespace Piccolo
{
    template<typename T>
    void ComponentArray<T>::removeData(Entity entity)
    {
        assert(m_entity_to_index.find(entity) != m_entity_to_index.end() && "Removing non-existent component.");

        // Copy element at end into deleted element's place to maintain density
        size_t indexOfRemovedEntity        = m_entity_to_index[entity];
        size_t indexOfLastElement          = m_size - 1;
        m_components[indexOfRemovedEntity] = m_components[indexOfLastElement];

        // Update map to point to moved spot
        Entity entityOfLastElement              = m_index_to_entity[indexOfLastElement];
        m_entity_to_index[entityOfLastElement]  = indexOfRemovedEntity;
        m_index_to_entity[indexOfRemovedEntity] = entityOfLastElement;

        m_entity_to_index.erase(entity);
        m_index_to_entity.erase(indexOfLastElement);
        --m_size;
    }

    template<typename T>
    void ComponentArray<T>::entityDestroyed(Entity entity)
    {
        if (m_entity_to_index.find(entity) != m_entity_to_index.end())
        {
            removeData(entity);
        }
    }
} // namespace Piccolo