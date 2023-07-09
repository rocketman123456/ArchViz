#pragma once

#include "runtime/core/math/math_type.h"

#include <volk.h>

namespace ArchViz
{
    struct Vertex
    {
        FVector2 pos;
        FVector3 color;
        FVector2 tex_coord;

        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription {};
            bindingDescription.binding   = 0;
            bindingDescription.stride    = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions {};

            attributeDescriptions[0].binding  = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format   = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset   = offsetof(Vertex, pos);

            attributeDescriptions[1].binding  = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset   = offsetof(Vertex, color);

            attributeDescriptions[2].binding  = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format   = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset   = offsetof(Vertex, tex_coord);

            return attributeDescriptions;
        }
    };

    struct UniformBufferObject
    {
        alignas(16) FMatrix4 model;
        alignas(16) FMatrix4 view;
        alignas(16) FMatrix4 proj;
    };
} // namespace ArchViz