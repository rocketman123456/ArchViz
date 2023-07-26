#pragma once
#include "runtime/core/base/macro.h"
#include "runtime/function/framework/ecs/component_array.h"

#include <any>
#include <memory>
#include <unordered_map>

namespace Piccolo
{
    class ComponentManager
    {
    public:
        template<typename T>
        void registerComponent()
        {
            const char* type_name = typeid(T).name();

            ASSERT(m_component_types.find(type_name) == m_component_types.end() && "Registering component type more than once.");

            m_component_types.insert({type_name, m_next_component_type});
            m_component.insert({type_name, std::make_shared<ComponentArray<T>>()});
            ++m_next_component_type;
        }

        template<typename T>
        ComponentType getComponentType()
        {
            const char* type_name = typeid(T).name();

            ASSERT(m_component_types.find(type_name) != m_component_types.end() && "Component not registered before use.");

            return m_component_types[type_name];
        }

        template<typename T>
        void addComponent(Entity entity, T component)
        {
            getComponentArray<T>()->insertData(entity, component);
        }

        template<typename T>
        void removeComponent(Entity entity)
        {
            getComponentArray<T>()->removeData(entity);
        }

        template<typename T>
        T& getComponent(Entity entity)
        {
            return getComponentArray<T>()->getData(entity);
        }

        void entityDestroyed(Entity entity);

    private:
        template<typename T>
        std::shared_ptr<ComponentArray<T>> getComponentArray()
        {
            const char* type_name = typeid(T).name();

            ASSERT(m_component_types.find(type_name) != m_component_types.end() && "Component not registered before use.");

            return std::static_pointer_cast<ComponentArray<T>>(m_component[type_name]);
        }

    private:
        std::unordered_map<const char*, ComponentType>                    m_component_types {};
        std::unordered_map<const char*, std::shared_ptr<IComponentArray>> m_component {};
        ComponentType                                                     m_next_component_type {};
    };
} // namespace Piccolo
