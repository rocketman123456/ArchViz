#pragma once

#include "runtime/core/base/hash.h"
#include "runtime/core/math/math_type.h"

#include <volk.h>

#include <array>

namespace ArchViz
{
    struct Light
    {
        FVector3 position;
        FVector3 color;

        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription {};
            bindingDescription.binding   = 0;
            bindingDescription.stride    = sizeof(Light);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions {};

            attributeDescriptions[0].binding  = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset   = offsetof(Light, position);

            attributeDescriptions[1].binding  = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset   = offsetof(Light, color);

            return attributeDescriptions;
        }
    };
} // namespace ArchViz

namespace std
{
    template<>
    struct hash<ArchViz::Light>
    {
        size_t operator()(const ArchViz::Light& light) const
        {
            size_t pos_hash = 0;
            ArchViz::hash_combine(pos_hash, light.position[0], light.position[1], light.position[2]);
            size_t color_hash = 0;
            ArchViz::hash_combine(color_hash, light.color[0], light.color[1], light.color[2]);
            return pos_hash ^ ((color_hash << 1) >> 1);
        }
    };

    template<>
    struct equal_to<ArchViz::Light>
    {
        bool operator()(const ArchViz::Light& lhs, const ArchViz::Light& rhs) const
        {
            const std::hash<ArchViz::Light> _hash_;
            return _hash_(lhs) == _hash_(rhs);
        }
    };
} // namespace std
