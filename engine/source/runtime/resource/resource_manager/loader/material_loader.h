#pragma once
#include "runtime/resource/resource_manager/loader/loader.h"
#include "runtime/resource/resource_manager/resource_handle.h"

#include "runtime/resource/res_type/components/material_res.h"
#include "runtime/resource/res_type/data/material_data.h"

#include <cstdint>
#include <string>

namespace ArchViz
{
    class MaterialLoader : public Loader<MaterialData, MaterialRes>
    {
    public:
        virtual ~MaterialLoader() = default;

        std::pair<std::shared_ptr<MaterialData>, size_t> createResource(const MaterialRes& create_info) override;
        std::pair<std::shared_ptr<MaterialData>, size_t> createResource(const std::string& uri) override;
    };
} // namespace ArchViz
