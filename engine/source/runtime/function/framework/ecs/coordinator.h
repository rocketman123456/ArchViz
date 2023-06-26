#pragma once

#include "runtime/function/framework/ecs/component_manager.h"
#include "runtime/function/framework/ecs/entity_manager.h"
#include "runtime/function/framework/ecs/system_manager.h"

#include <memory>

namespace Piccolo
{
    class Coordinator
    {
    public:
        void init();
        void clear();

        // Entity methods
        Entity createEntity();
        void   destroyEntity(Entity entity);

        // Component methods
        template<typename T>
        void registerComponent()
        {
            m_component_manager->registerComponent<T>();
        }

        template<typename T>
        void addComponent(Entity entity, T component)
        {
            m_component_manager->addComponent<T>(entity, component);

            auto signature = m_entity_manager->getSignature(entity);
            signature.set(m_component_manager->getComponentType<T>(), true);
            m_entity_manager->setSignature(entity, signature);

            m_system_manager->entitySignatureChanged(entity, signature);
        }

        template<typename T>
        void removeComponent(Entity entity)
        {
            m_component_manager->removeComponent<T>(entity);

            auto signature = m_entity_manager->getSignature(entity);
            signature.set(m_component_manager->getComponentType<T>(), false);
            m_entity_manager->setSignature(entity, signature);

            m_system_manager->entitySignatureChanged(entity, signature);
        }

        template<typename T>
        T& getComponent(Entity entity)
        {
            return m_component_manager->getComponent<T>(entity);
        }

        template<typename T>
        ComponentType getComponentType()
        {
            return m_component_manager->getComponentType<T>();
        }

        // System methods
        template<typename T>
        std::shared_ptr<T> registerSystem()
        {
            return m_system_manager->registerSystem<T>();
        }

        template<typename T>
        void setSystemSignature(Signature signature)
        {
            m_system_manager->setSignature<T>(signature);
        }

    private:
        std::unique_ptr<ComponentManager> m_component_manager;
        std::unique_ptr<EntityManager>    m_entity_manager;
        std::unique_ptr<SystemManager>    m_system_manager;
    };

    extern Coordinator g_coordinator;
} // namespace Piccolo
