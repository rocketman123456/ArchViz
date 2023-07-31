#include "runtime/resource/resource_manager/loader/material_loader.h"

#include "runtime/function/global/global_context.h"

namespace ArchViz
{
    std::pair<std::shared_ptr<MaterialData>, size_t> MaterialLoader::createResource(const MaterialRes& create_info)
    {
        std::shared_ptr<MaterialData> material;
        return {material, sizeof(MaterialData)};
    }

    std::pair<std::shared_ptr<MaterialData>, size_t> MaterialLoader::createResource(const std::string& uri)
    {
        std::shared_ptr<MaterialData> material;
        return {material, sizeof(MaterialData)};
    }
} // namespace ArchViz
