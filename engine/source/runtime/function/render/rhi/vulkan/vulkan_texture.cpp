#include "runtime/function/render/rhi/vulkan/vulkan_texture.h"
#include "runtime/function/render/rhi/vulkan/vulkan_buffer_utils.h"
#include "runtime/function/render/rhi/vulkan/vulkan_device.h"
#include "runtime/function/render/rhi/vulkan/vulkan_texture_utils.h"
#include "runtime/function/render/rhi/vulkan/vulkan_utils.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/core/base/macro.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <filesystem>

namespace ArchViz
{
    void VulkanTexture::createTextureImageFromFile(const std::string& image_uri)
    {
        std::filesystem::path root       = m_config_manager->getRootFolder();
        std::filesystem::path image_path = root / image_uri;

        int          tex_width, tex_height, tex_channels;
        stbi_uc*     pixels     = stbi_load(image_path.generic_string().c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
        VkDeviceSize image_size = tex_width * tex_height * 4;

        m_width  = tex_width;
        m_height = tex_height;

        // stbi_load_from_memory(stbi_uc const *buffer, int len, int *x, int *y, int *comp, int req_comp)

        if (!pixels)
        {
            // TODO : use default image instead
            LOG_FATAL("failed to load texture image!");
        }

        VkBuffer       staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        auto           usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        auto           flag  = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        VulkanBufferUtils::createBuffer(m_device, image_size, usage, flag, staging_buffer, staging_buffer_memory);

        void* data;
        vkMapMemory(m_device->m_device, staging_buffer_memory, 0, image_size, 0, &data);
        {
            memcpy(data, pixels, static_cast<size_t>(image_size));
        }
        vkUnmapMemory(m_device->m_device, staging_buffer_memory);

        stbi_image_free(pixels);

        m_format          = VK_FORMAT_R8G8B8A8_SRGB;
        m_tiling          = VK_IMAGE_TILING_OPTIMAL;
        m_usage           = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        m_memory_property = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        VulkanTextureUtils::createImage(m_device, tex_width, tex_height, m_format, m_tiling, m_usage, m_memory_property, m_image, m_device_memory);

        VulkanTextureUtils::transitionImageLayout(m_device, m_command_pool, m_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        VulkanTextureUtils::copyBufferToImage(m_device, m_command_pool, staging_buffer, m_image, static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height));
        VulkanTextureUtils::transitionImageLayout(m_device, m_command_pool, m_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(m_device->m_device, staging_buffer, nullptr);
        vkFreeMemory(m_device->m_device, staging_buffer_memory, nullptr);
    }

    void VulkanTexture::createTextureImageFromMemory(const uint8_t* pixels, const VkDeviceSize image_size, VkFormat format, uint32_t width, uint32_t height)
    {
        VkBuffer       staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        auto           usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        auto           flag  = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        VulkanBufferUtils::createBuffer(m_device, image_size, usage, flag, staging_buffer, staging_buffer_memory);

        void* data;
        vkMapMemory(m_device->m_device, staging_buffer_memory, 0, image_size, 0, &data);
        {
            memcpy(data, pixels, static_cast<size_t>(image_size));
        }
        vkUnmapMemory(m_device->m_device, staging_buffer_memory);

        m_format          = format;
        m_tiling          = VK_IMAGE_TILING_OPTIMAL;
        m_usage           = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        m_memory_property = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        VulkanTextureUtils::createImage(m_device, width, height, m_format, m_tiling, m_usage, m_memory_property, m_image, m_device_memory);

        VulkanTextureUtils::transitionImageLayout(m_device, m_command_pool, m_image, m_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        VulkanTextureUtils::copyBufferToImage(m_device, m_command_pool, staging_buffer, m_image, width, height);
        VulkanTextureUtils::transitionImageLayout(m_device, m_command_pool, m_image, m_format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(m_device->m_device, staging_buffer, nullptr);
        vkFreeMemory(m_device->m_device, staging_buffer_memory, nullptr);
    }

    void VulkanTexture::createTextureImageView()
    {
        // create image view
        m_view = VulkanTextureUtils::createImageView(m_device, m_image, m_format, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    void VulkanTexture::createTextureSampler()
    {
        VkPhysicalDeviceProperties properties {};
        vkGetPhysicalDeviceProperties(m_device->m_physical_device, &properties);

        VkSamplerCreateInfo sampler_info {};
        sampler_info.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter               = VK_FILTER_LINEAR;
        sampler_info.minFilter               = VK_FILTER_LINEAR;
        sampler_info.addressModeU            = m_address_mode;
        sampler_info.addressModeV            = m_address_mode;
        sampler_info.addressModeW            = m_address_mode;
        sampler_info.anisotropyEnable        = VK_TRUE;
        sampler_info.maxAnisotropy           = properties.limits.maxSamplerAnisotropy;
        sampler_info.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable           = VK_FALSE;
        sampler_info.compareOp               = VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        // sampler_info.anisotropyEnable        = VK_FALSE;
        // sampler_info.maxAnisotropy           = 1.0f;

        if (vkCreateSampler(m_device->m_device, &sampler_info, nullptr, &m_sampler) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create texture sampler!");
        }
    }

    void VulkanTexture::initizlize(const std::string& image_uri)
    {
        createTextureImageFromFile(image_uri);
        createTextureImageView();
        createTextureSampler();
    }

    void VulkanTexture::initialize(const uint8_t* pixels, const VkDeviceSize image_size, VkFormat format, uint32_t width, uint32_t height)
    {
        createTextureImageFromMemory(pixels, image_size, format, width, height);
        createTextureImageView();
        createTextureSampler();
    }

    void VulkanTexture::clear()
    {
        vkDestroySampler(m_device->m_device, m_sampler, nullptr);
        vkDestroyImageView(m_device->m_device, m_view, nullptr);
        vkFreeMemory(m_device->m_device, m_device_memory, nullptr);
        vkDestroyImage(m_device->m_device, m_image, nullptr);
    }

} // namespace ArchViz
