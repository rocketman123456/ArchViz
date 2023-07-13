#pragma once

#include <volk.h>

#include <memory>

namespace ArchViz
{
    class VulkanDevice;

    class VulkanTextureUtils
    {
    public:
        static void createImage(std::shared_ptr<VulkanDevice> device,
                                uint32_t                      width,
                                uint32_t                      height,
                                uint32_t                      mip_level,
                                VkFormat                      format,
                                VkImageTiling                 tiling,
                                VkImageUsageFlags             usage,
                                VkMemoryPropertyFlags         properties,
                                VkImage&                      image,
                                VkDeviceMemory&               image_memory);

        static VkImageView createImageView(std::shared_ptr<VulkanDevice> device, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t mip_level);

        static void
        transitionImageLayout(std::shared_ptr<VulkanDevice> device, VkCommandPool command_pool, VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout, uint32_t mip_level);

        static void copyBufferToImage(std::shared_ptr<VulkanDevice> device, VkCommandPool command_pool, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

        static void generateMipmaps(std::shared_ptr<VulkanDevice> device, VkCommandPool command_pool, VkImage image, VkFormat image_format, int32_t tex_width, int32_t tex_height, uint32_t mip_levels);
    };
} // namespace ArchViz
