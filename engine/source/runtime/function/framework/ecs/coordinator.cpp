#include "runtime/function/framework/ecs/coordinator.h"

namespace Piccolo
{
    Coordinator g_coordinator = {};

    void Coordinator::init()
    {
        m_component_manager = std::make_unique<ComponentManager>();
        m_entity_manager    = std::make_unique<EntityManager>();
        m_system_manager    = std::make_unique<SystemManager>();
    }

    void Coordinator::clear()
    {
        m_component_manager.reset();
        m_entity_manager.reset();
        m_system_manager.reset();
    }

    Entity Coordinator::createEntity() { return m_entity_manager->createEntity(); }

    void Coordinator::destroyEntity(Entity entity)
    {
        m_entity_manager->destroyEntity(entity);
        m_component_manager->entityDestroyed(entity);
        m_system_manager->entityDestroyed(entity);
    }
} // namespace Piccolo