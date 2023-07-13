#include "runtime/function/render/rhi/vulkan/vulkan_texture.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_buffer_utils.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_texture_utils.h"
#include "runtime/function/render/rhi/vulkan/utils/vulkan_utils.h"
#include "runtime/function/render/rhi/vulkan/vulkan_device.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/core/base/macro.h"

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

        m_width      = tex_width;
        m_height     = tex_height;
        m_channel    = tex_channels;
        m_mip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(m_width, m_height)))) + 1;
        m_format     = VK_FORMAT_R8G8B8A8_SRGB;

        if (!pixels)
        {
            // TODO : use default image instead
            LOG_FATAL("failed to load texture image!");
        }

        createTextureImage(pixels, image_size);

        stbi_image_free(pixels);
    }

    void VulkanTexture::createTextureImageFromMemory(const uint8_t* image_data, const VkDeviceSize size)
    {
        int          tex_width, tex_height, tex_channels;
        stbi_uc*     pixels     = stbi_load_from_memory(pixels, size, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
        VkDeviceSize image_size = tex_width * tex_height * 4;

        m_width      = tex_width;
        m_height     = tex_height;
        m_channel    = tex_channels;
        m_mip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(m_width, m_height)))) + 1;
        m_format     = VK_FORMAT_R8G8B8A8_SRGB;

        createTextureImage(pixels, image_size);

        stbi_image_free(pixels);
    }

    void VulkanTexture::createTextureImageFromMemory(const uint8_t* pixels, const VkDeviceSize image_size, VkFormat format, uint32_t width, uint32_t height)
    {
        m_width      = width;
        m_height     = height;
        m_mip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(m_width, m_height)))) + 1;
        m_format     = format;

        if (format >= VK_FORMAT_R8G8B8A8_UNORM && format <= VK_FORMAT_B8G8R8A8_SRGB)
        {
            m_channel = 4;
        }
        else if (format >= VK_FORMAT_R8G8B8_UNORM && format <= VK_FORMAT_B8G8R8_SRGB)
        {
            m_channel = 3;
        }
        else if (format >= VK_FORMAT_R8G8_UNORM && format <= VK_FORMAT_R8G8_SRGB)
        {
            m_channel = 3;
        }
        else if (format >= VK_FORMAT_R8_UNORM && format <= VK_FORMAT_R8_SRGB)
        {
            m_channel = 3;
        }

        createTextureImage(pixels, image_size);
    }

    void VulkanTexture::createTextureImage(const uint8_t* pixels, const size_t image_size)
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

        m_tiling          = VK_IMAGE_TILING_OPTIMAL;
        m_usage           = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        m_memory_property = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        m_image_layout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VulkanTextureUtils::createImage(m_device, m_width, m_height, m_mip_levels, m_format, m_tiling, m_usage, m_memory_property, m_image, m_device_memory);

        VulkanTextureUtils::transitionImageLayout(m_device, m_command_pool, m_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mip_levels);
        VulkanTextureUtils::copyBufferToImage(m_device, m_command_pool, staging_buffer, m_image, static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height));
        // VulkanTextureUtils::transitionImageLayout(m_device, m_command_pool, m_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_image_layout, m_mip_levels);

        VulkanBufferUtils::destroyBuffer(m_device, staging_buffer, staging_buffer_memory);

        VulkanTextureUtils::generateMipmaps(m_device, m_command_pool, m_image, m_format, m_width, m_height, m_mip_levels);
    }

    void VulkanTexture::createTextureImageView()
    {
        // create image view
        m_view = VulkanTextureUtils::createImageView(m_device, m_image, m_format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
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
        sampler_info.minLod                  = -1000;
        sampler_info.maxLod                  = 1000;
        // sampler_info.anisotropyEnable        = VK_FALSE;
        // sampler_info.maxAnisotropy           = 1.0f;

        if (vkCreateSampler(m_device->m_device, &sampler_info, nullptr, &m_sampler) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create texture sampler!");
        }
    }

    void VulkanTexture::createDescriptorSet()
    {
        if (m_descriptor_pool == VK_NULL_HANDLE || m_descriptor_set_layout == VK_NULL_HANDLE)
        {
            return;
        }

        // create descriptor set
        VkDescriptorSetAllocateInfo alloc_info = {};
        alloc_info.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool              = m_descriptor_pool;
        alloc_info.descriptorSetCount          = 1;
        alloc_info.pSetLayouts                 = &m_descriptor_set_layout;
        if (vkAllocateDescriptorSets(m_device->m_device, &alloc_info, &m_descriptor_set) != VK_SUCCESS)
        {
            LOG_FATAL("failed to create vulkan texture descriptor set");
        }

        // update descriptor set
        VkDescriptorImageInfo desc_image[1] {};
        desc_image[0].sampler     = m_sampler;
        desc_image[0].imageView   = m_view;
        desc_image[0].imageLayout = m_image_layout;

        VkWriteDescriptorSet write_desc[1] {};
        write_desc[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_desc[0].dstSet          = m_descriptor_set;
        write_desc[0].descriptorCount = 1;
        write_desc[0].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write_desc[0].pImageInfo      = desc_image;

        vkUpdateDescriptorSets(m_device->m_device, 1, write_desc, 0, nullptr);
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

    void VulkanTexture::initialize(const uint8_t* pixels, const VkDeviceSize image_size)
    {
        createTextureImageFromMemory(pixels, image_size);
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
