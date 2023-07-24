#pragma once

#include <volk.h>

#include <memory>
#include <string>

namespace ArchViz
{
    class AssetManager;
    class ConfigManager;
    class VulkanDevice;

    class VulkanTexture
    {
    public:
        void initizlize(const std::string& image_uri);                                                                           // from file
        void initialize(const uint8_t* image_data, const VkDeviceSize size);                                                     // no decode
        void initialize(const uint8_t* pixels, const VkDeviceSize image_size, VkFormat format, uint32_t width, uint32_t height); // after decode

        void clear();

        void createDescriptorSet();

    private:
        void createTextureImageFromFile(const std::string& image_uri);
        void createTextureImageFromMemory(const uint8_t* image_data, const VkDeviceSize size);
        void createTextureImageFromMemory(const uint8_t* pixels, const VkDeviceSize image_size, VkFormat format, uint32_t width, uint32_t height);
        void createTextureImage(const uint8_t* pixels, const size_t image_size);
        void createTextureImageView();
        void createTextureSampler();

    public:
        std::shared_ptr<VulkanDevice>  m_device;
        std::shared_ptr<AssetManager>  m_asset_manager;
        std::shared_ptr<ConfigManager> m_config_manager;

        VkCommandPool m_command_pool;

        VkDescriptorSetLayout m_descriptor_set_layout = VK_NULL_HANDLE;
        VkDescriptorPool      m_descriptor_pool       = VK_NULL_HANDLE;
        VkDescriptorSet       m_descriptor_set        = VK_NULL_HANDLE; // for imgui imgui show

        VkImage               m_image;
        VkImageView           m_view;
        VkSampler             m_sampler;
        VkImageLayout         m_image_layout;
        VkDeviceMemory        m_device_memory;
        VkDescriptorImageInfo m_descriptor;

        VkFormat              m_format;
        VkImageTiling         m_tiling;
        VkImageUsageFlags     m_usage;
        VkMemoryPropertyFlags m_memory_property;
        VkSamplerAddressMode  m_address_mode;

        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_channel;
        uint32_t m_mip_levels = 1;
        uint32_t m_layer_count;
    };

    class VulkanTexture2D : public VulkanTexture
    {};

    class VulkanTexture2DArray : public VulkanTexture
    {};

    class VulkanTextureCubeMap : public VulkanTexture
    {};
} // namespace ArchViz
