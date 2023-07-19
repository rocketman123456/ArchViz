#pragma once
#include "runtime/core/base/hash.h"
#include "runtime/core/math/math_type.h"

#include <volk.h>

#include <array>

namespace ArchViz
{
    struct Particle
    {
        FVector2 position;
        FVector2 velocity;
        FVector4 color;

        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription {};
            bindingDescription.binding   = 0;
            bindingDescription.stride    = sizeof(Particle);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions {};

            attributeDescriptions[0].binding  = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format   = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset   = offsetof(Particle, position);

            attributeDescriptions[1].binding  = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[1].offset   = offsetof(Particle, color);

            return attributeDescriptions;
        }
    };
} // namespace ArchViz

namespace std
{
    template<>
    struct hash<ArchViz::Particle>
    {
        size_t operator()(ArchViz::Particle const& particle) const
        {
            size_t pos_hash = 0;
            ArchViz::hash_combine(pos_hash, particle.position[0], particle.position[1], particle.position[2]);
            size_t vel_hash = 0;
            ArchViz::hash_combine(vel_hash, particle.velocity[0], particle.velocity[1], particle.velocity[2]);
            size_t color_hash = 0;
            ArchViz::hash_combine(color_hash, particle.color[0], particle.color[1], particle.color[2], particle.color[3]);
            return pos_hash ^ ((vel_hash << 1) >> 1) ^ (color_hash << 1);
        }
    };
} // namespace std
