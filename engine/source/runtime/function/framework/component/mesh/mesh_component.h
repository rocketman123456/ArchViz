#pragma once
#include "runtime/function/framework/component/component.h"
#include "runtime/resource/res_type/components/mesh_res.h"

#include <memory>

namespace ArchViz
{
    class ResourceManager;

    REFLECTION_TYPE(MeshComponent)
    CLASS(MeshComponent : public Component, WhiteListFields)
    {
        REFLECTION_BODY(MeshComponent)
    public:
        MeshComponent() {};

        void postLoadResource(std::weak_ptr<GObject> parent_object) override;

        void tick(float delta_time) override;

    private:
        META(Enable)
        MeshComponentRes m_mesh_res;

        std::weak_ptr<ResourceManager> m_resource_manager;
    };
} // namespace ArchViz
