#include "runtime/function/framework/ecs/component_manager.h"

namespace Piccolo
{
    void ComponentManager::entityDestroyed(Entity entity)
    {
        for (auto const& pair : m_component)
        {
            auto const& component = pair.second;
            component->entityDestroyed(entity);
        }
    }
} // namespace Piccolo