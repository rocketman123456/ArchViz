#include "runtime/resource/resource_manager/loader/texture_loader.h"

#include "runtime/function/global/global_context.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"
#include "runtime/resource/resource_manager/resource_manager.h"

#include "runtime/resource/res_type/data/material_data.h"

#include "runtime/core/base/macro.h"

#include <stb_image.h>

#include <filesystem>

namespace ArchViz
{
    std::pair<std::shared_ptr<TextureData>, size_t> TextureLoader::createResource(const TextureRes& create_info)
    {
        int tex_width, tex_height, tex_channels;

        std::shared_ptr<TextureData> texture;
        return {texture, 0};
    }

    std::pair<std::shared_ptr<TextureData>, size_t> TextureLoader::createResource(const std::string& uri)
    {
        int tex_width, tex_height, tex_channels;

        std::shared_ptr<TextureData> texture;
        return {texture, 0};
    }

    std::shared_ptr<TextureData> TextureLoader::loadFromFile(const std::string& uri, int* width, int* height, int* channel)
    {
        std::filesystem::path root       = g_runtime_global_context.m_config_manager->getRootFolder();
        std::filesystem::path image_path = root / uri;

        std::shared_ptr<TextureData> texture = std::make_shared<TextureData>();

        // int          tex_width, tex_height, tex_channels;
        // stbi_uc*     pixels     = stbi_load(image_path.generic_string().c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
        // VkDeviceSize image_size = tex_width * tex_height * 4;

        // m_width      = tex_width;
        // m_height     = tex_height;
        // m_channel    = tex_channels;
        // m_mip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(m_width, m_height)))) + 1;
        // m_format     = VK_FORMAT_R8G8B8A8_SRGB;

        // if (!pixels)
        //{
        //     // TODO : use default image instead
        //     LOG_FATAL("failed to load texture image!");
        // }

        // stbi_image_free(pixels);

        return nullptr;
    }
} // namespace ArchViz
