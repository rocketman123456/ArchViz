#pragma once
#include "runtime/resource/resource_manager/loader/loader.h"

#include "runtime/resource/res_type/components/material_res.h"
#include "runtime/resource/res_type/data/material_data.h"

#include <memory>

namespace ArchViz
{
    class ResourceManager;

    class TextureLoader : public Loader<TextureData, TextureRes>
    {
    public:
        virtual ~TextureLoader() = default;

        std::pair<std::shared_ptr<TextureData>, size_t> createResource(const TextureRes& create_info) override;
        std::pair<std::shared_ptr<TextureData>, size_t> createResource(const std::string& uri) override;

    private:
        std::shared_ptr<TextureData> loadFromFile(const std::string& uri);
    };
} // namespace ArchViz
