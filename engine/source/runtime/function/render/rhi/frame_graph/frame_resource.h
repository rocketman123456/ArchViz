#pragma once

#include <volk.h>

#include <memory>

namespace ArchViz
{
    class VulkanBuffer;
    class VulkanTexture;

    struct FrameGraphResourceInfo
    {
        bool external = false;

        union
        {
            struct
            {
                size_t                size;
                VkBufferUsageFlags    flags;
                VkMemoryPropertyFlags properties;

                std::shared_ptr<VulkanBuffer> buffer;
            } buffer;

            struct
            {
                uint32_t width;
                uint32_t height;
                uint32_t depth;

                VkFormat          format;
                VkImageUsageFlags flags;

                //RenderPassOperation::Enum load_op;

                std::shared_ptr<VulkanTexture> texture;
            } texture;
        };
    };
} // namespace ArchViz
