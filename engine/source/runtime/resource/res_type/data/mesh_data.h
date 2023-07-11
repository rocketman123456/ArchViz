#pragma once
#include "runtime/core/meta/reflection/reflection.h"

#include <string>
#include <vector>

namespace ArchViz
{
    REFLECTION_TYPE(MeshVertex)
    CLASS(MeshVertex, Fields)
    {
        REFLECTION_BODY(MeshVertex);

    public:
        float px;
        float py;
        float pz;
        float nx;
        float ny;
        float nz;
        float tx;
        float ty;
        float tz;
        float u;
        float v;
    };

    REFLECTION_TYPE(SkeletonBinding)
    CLASS(SkeletonBinding, Fields)
    {
        REFLECTION_BODY(SkeletonBinding);

    public:
        int32_t index0;
        int32_t index1;
        int32_t index2;
        int32_t index3;
        float   weight0;
        float   weight1;
        float   weight2;
        float   weight3;
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

} // namespace ArchViz