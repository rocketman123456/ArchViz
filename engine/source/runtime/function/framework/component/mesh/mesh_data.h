#pragma once
#include "runtime/core/math/math_type.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ArchViz
{
    enum class PrimitiveType
    {
        Point,
        Line,
        Triangle,
        Quad
    };

    struct RawVertex
    {
        FVector3 pos;
        FVector3 normal;
        FVector3 tangents;
        FVector2 tex_coord;
    };

    struct RawVertexBuffer
    {
        std::vector<RawVertex> vertices;
    };

    struct RawIndexBuffer
    {
        PrimitiveType         primitive_type {PrimitiveType::Triangle};
        uint32_t              primitive_count {0};
        std::vector<uint32_t> indices;
    };

    struct RawMaterialUri
    {
        std::string base_color_file;
        std::string metallic_roughness_file;
        std::string normal_file;
        std::string occlusion_file;
        std::string emissive_file;
    };

    struct RawMeshData
    {
        RawVertexBuffer vertex_buffer;
        RawIndexBuffer  index_buffer;
        RawMaterialUri  material_uri;
    };
} // namespace ArchViz
