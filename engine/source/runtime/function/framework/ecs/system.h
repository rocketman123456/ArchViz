#pragma once
#include "runtime/function/framework/ecs/types.h"

#include <set>

namespace ArchViz
{
    class System
    {
    public:
        // Example:
        // for (auto const& entity : mEntities)
        // {
        //     auto& rigidBody = gCoordinator.GetComponent<RigidBody>(entity);
        //     auto& transform = gCoordinator.GetComponent<Transform>(entity);
        //     // Forces
        //     auto const& gravity = gCoordinator.GetComponent<Gravity>(entity);
        //     transform.position += rigidBody.velocity * dt;
        //     rigidBody.velocity += gravity.force * dt;
        // }
        virtual void tick(float dt) = 0;

        std::set<Entity> m_entities;
    };
} // namespace ArchViz
