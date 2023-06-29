#pragma once

#include <volk.h>

#include <memory>

namespace ArchViz
{
    class VulkanDevice;

    class Texture
    {
    public:
        std::shared_ptr<VulkanDevice> m_device;

        VkImage               m_image;
        VkImageLayout         m_image_layout;
        VkDeviceMemory        m_device_memory;
        VkImageView           m_view;
        uint32_t              m_width, m_height;
        uint32_t              m_mip_levels;
        uint32_t              m_layer_count;
        VkDescriptorImageInfo m_descriptor;
    };

    class Texture2D : public Texture
    {};

    class Texture2DArray : public Texture
    {};

    class TextureCubeMap : public Texture
    {};
} // namespace ArchViz
