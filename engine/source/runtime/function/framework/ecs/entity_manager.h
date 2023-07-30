#pragma once
#include "runtime/function/framework/ecs/types.h"

#include <array>
#include <cassert>
#include <queue>

namespace ArchViz
{
    class EntityManager
    {
    public:
        EntityManager();

        Entity createEntity();
        void   destroyEntity(Entity entity);

        void      setSignature(Entity entity, Signature signature);
        Signature getSignature(Entity entity);

    private:
        std::queue<Entity>                  m_available_entities {};
        std::array<Signature, MAX_ENTITIES> m_signatures {};
        uint32_t                            m_living_entity_count {};
    };
} // namespace ArchViz
