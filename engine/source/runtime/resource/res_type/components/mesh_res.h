#pragma once
#include "runtime/core/math/transform.h"
#include "runtime/core/meta/reflection/reflection.h"

namespace ArchViz
{
    REFLECTION_TYPE(SubMeshRes)
    CLASS(SubMeshRes, Fields)
    {
        REFLECTION_BODY(SubMeshRes);

    public:
        std::string m_obj_file_ref;
        std::string m_material;
        Transform   m_transform;
    };

    REFLECTION_TYPE(MeshComponentRes)
    CLASS(MeshComponentRes, Fields)
    {
        REFLECTION_BODY(MeshComponentRes);

    public:
        std::vector<SubMeshRes> m_sub_meshes;
    };
} // namespace ArchViz