#include "runtime/resource/resource_manager/loader/texture_loader.h"

#include "runtime/function/global/global_context.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/core/base/macro.h"

#include <stb_image.h>

#include <filesystem>

namespace ArchViz
{
    std::pair<std::shared_ptr<TextureData>, size_t> TextureLoader::createResource(const TextureRes& create_info)
    {
        std::shared_ptr<TextureData> texture = loadFromFile(create_info.m_texture_uri);
        if (texture != nullptr)
        {
            return {texture, texture->m_data.size()};
        }
        return {nullptr, 0};
    }

    std::pair<std::shared_ptr<TextureData>, size_t> TextureLoader::createResource(const std::string& uri)
    {
        std::shared_ptr<TextureData> texture = loadFromFile(uri);
        if (texture != nullptr)
        {
            return {texture, texture->m_data.size()};
        }
        return {nullptr, 0};
    }

    std::shared_ptr<TextureData> TextureLoader::loadFromFile(const std::string& uri)
    {
        std::filesystem::path root       = g_runtime_global_context.m_config_manager->getRootFolder();
        std::filesystem::path image_path = root / uri;

        std::shared_ptr<TextureData> texture = std::make_shared<TextureData>();

        // int          tex_width, tex_height, tex_channels;
        stbi_uc* pixels     = stbi_load(image_path.generic_string().c_str(), &texture->m_width, &texture->m_height, &texture->m_channel, STBI_rgb_alpha);
        size_t   image_size = texture->m_width * texture->m_height * 4;

        if (!pixels)
        {
            LOG_ERROR("failed to load texture image!");
            return nullptr;
        }

        texture->m_data.resize(image_size);
        memcpy(texture->m_data.data(), pixels, image_size);

        return texture;
    }
} // namespace ArchViz
