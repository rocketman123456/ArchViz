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

    //struct RawMeshVertex
    //{
    //    FVector3 postion {};
    //    FVector3 normal {};
    //    FVector3 tangent {};
    //    FVector2 uv {};
    //};

    //struct RawSubMeshData
    //{
    //    uint64_t first_vertex;
    //    uint64_t vertex_count;
    //    uint64_t first_index;
    //    uint64_t index_count;
    //};

    //struct RawMeshData
    //{
    //    std::vector<RawMeshVertex> vertices;
    //    std::vector<uint32_t>      indices;

    //    // TODO : 
    //    std::vector<RawSubMeshData> sub_meshes;
    //};

} // namespace ArchViz