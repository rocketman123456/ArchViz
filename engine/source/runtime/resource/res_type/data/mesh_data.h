#pragma once
#include "runtime/core/meta/reflection/reflection.h"

#include "runtime/core/math/vector/vector2.h"
#include "runtime/core/math/vector/vector3.h"
#include "runtime/core/math/vector/vector4.h"

#include "runtime/core/math/math_type.h"

#include <string>
#include <vector>

namespace ArchViz
{
    REFLECTION_TYPE(MeshVertex)
    CLASS(MeshVertex, Fields)
    {
        REFLECTION_BODY(MeshVertex);

    public:
        Vector3 postion {};
        Vector3 normal {};
        Vector3 tangent {};
        Vector2 uv {};
    };

    // TODO : add sub mesh

    REFLECTION_TYPE(SkeletonBinding)
    CLASS(SkeletonBinding, Fields)
    {
        REFLECTION_BODY(SkeletonBinding);

    public:
        Vector4 index;
        Vector4 weight;
    };

    REFLECTION_TYPE(MeshData)
    CLASS(MeshData, Fields)
    {
        REFLECTION_BODY(MeshData);

    public:
        std::vector<MeshVertex>      vertex_buffer;
        std::vector<int>             index_buffer;
        std::vector<SkeletonBinding> bind;
    };

    // TODO : use same structure in gltf to build data
    struct ModelData
    {
        //std::vector<ResHandle> mesh_data {};
        //std::vector<ResHandle> material_data {};
    };

} // namespace ArchViz