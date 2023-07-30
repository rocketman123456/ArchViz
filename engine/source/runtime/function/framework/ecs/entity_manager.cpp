#include "runtime/function/framework/ecs/entity_manager.h"
#include "runtime/core/base/macro.h"

namespace ArchViz
{
    EntityManager::EntityManager()
    {
        for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
        {
            m_available_entities.push(entity);
        }
    }

    Entity EntityManager::createEntity()
    {
        ASSERT(m_living_entity_count < MAX_ENTITIES && "Too many entities in existence.");

        Entity id = m_available_entities.front();
        m_available_entities.pop();
        ++m_living_entity_count;

        return id;
    }

    void EntityManager::destroyEntity(Entity entity)
    {
        ASSERT(entity < MAX_ENTITIES && "Entity out of range.");

        m_signatures[entity].reset();
        m_available_entities.push(entity);
        --m_living_entity_count;
    }

    void EntityManager::setSignature(Entity entity, Signature signature)
    {
        ASSERT(entity < MAX_ENTITIES && "Entity out of range.");
        m_signatures[entity] = signature;
    }

    Signature EntityManager::getSignature(Entity entity)
    {
        ASSERT(entity < MAX_ENTITIES && "Entity out of range.");
        return m_signatures[entity];
    }
} // namespace ArchViz
