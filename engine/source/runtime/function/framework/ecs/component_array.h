#pragma once
#include "runtime/core/base/macro.h"
#include "runtime/function/framework/ecs/types.h"

#include <array>
#include <cassert>
#include <unordered_map>

namespace Piccolo
{
    // The virtual inheritance of IComponentArray is unfortunate but, as far as I can tell, unavoidable. As seen later, we'll have a list of every ComponentArray (one per component type), and we need
    // to notify all of them when an entity is destroyed so that it can remove the entity's data if it exists. The only way to keep a list of multiple templated types is to keep a list of their common
    // interface so that we can call EntityDestroyed() on all of them.
    class IComponentArray
    {
    public:
        virtual ~IComponentArray() = default;

        virtual void entityDestroyed(Entity entity) = 0;
    };

    template<typename T>
    class ComponentArray : public IComponentArray
    {
    public:
        void insertData(Entity entity, T component)
        {
            ASSERT(m_entity_to_index.find(entity) == m_entity_to_index.end() && "Component added to same entity more than once.");
            // Put new entry at end
            size_t newIndex             = m_size;
            m_entity_to_index[entity]   = newIndex;
            m_index_to_entity[newIndex] = entity;
            m_components[newIndex]      = component;
            ++m_size;
        }

        T& getData(Entity entity)
        {
            ASSERT(m_entity_to_index.find(entity) != m_entity_to_index.end() && "Retrieving non-existent component.");
            return m_components[m_entity_to_index[entity]];
        }

        void removeData(Entity entity);
        void entityDestroyed(Entity entity) override;

    private:
        std::array<T, MAX_ENTITIES>        m_components {};
        std::unordered_map<Entity, size_t> m_entity_to_index {};
        std::unordered_map<size_t, Entity> m_index_to_entity {};
        size_t                             m_size {};
    };
} // namespace Piccolo
