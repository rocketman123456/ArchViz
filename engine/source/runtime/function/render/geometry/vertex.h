#pragma once
#include "runtime/core/base/hash.h"
#include "runtime/core/math/math_type.h"

#include <volk.h>

#include <array>

namespace ArchViz
{
    struct Vertex
    {
        FVector3 pos;
        FVector3 color;
        FVector3 normal;
        FVector2 tex_coord;

        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription {};
            bindingDescription.binding   = 0;
            bindingDescription.stride    = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions {};

            attributeDescriptions[0].binding  = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset   = offsetof(Vertex, pos);

            attributeDescriptions[1].binding  = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset   = offsetof(Vertex, color);

            attributeDescriptions[2].binding  = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[2].offset   = offsetof(Vertex, normal);

            attributeDescriptions[3].binding  = 0;
            attributeDescriptions[3].location = 3;
            attributeDescriptions[3].format   = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[3].offset   = offsetof(Vertex, tex_coord);

            return attributeDescriptions;
        }

        bool operator==(const Vertex& other) const { return pos == other.pos && color == other.color && tex_coord == other.tex_coord; }
    };

} // namespace ArchViz

namespace std
{
    template<>
    struct hash<ArchViz::Vertex>
    {
        size_t operator()(ArchViz::Vertex const& vertex) const
        {
            size_t pos_hash = 0;
            ArchViz::hash_combine(pos_hash, vertex.pos[0], vertex.pos[1], vertex.pos[2]);
            size_t color_hash = 0;
            ArchViz::hash_combine(color_hash, vertex.color[0], vertex.color[1], vertex.color[2]);
            size_t tex_hash = 0;
            ArchViz::hash_combine(tex_hash, vertex.tex_coord[0], vertex.tex_coord[1]);
            return pos_hash ^ ((color_hash << 1) >> 1) ^ (tex_hash << 1);
        }
    };
} // namespace std
