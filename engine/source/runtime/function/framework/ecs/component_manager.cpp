#include "runtime/function/framework/ecs/component_manager.h"

namespace ArchViz
{
    void ComponentManager::entityDestroyed(Entity entity)
    {
        for (auto const& pair : m_component)
        {
            auto const& component = pair.second;
            component->entityDestroyed(entity);
        }
    }
} // namespace ArchViz