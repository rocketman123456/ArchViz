#pragma once
#include "runtime/resource/resource_manager/loader/loader.h"
#include "runtime/resource/resource_manager/resource_handle.h"

#include "runtime/resource/res_type/components/mesh_res.h"
#include "runtime/resource/res_type/data/mesh_data.h"

#include <cstdint>
#include <string>

namespace ArchViz
{
    class ModelLoader : public ILoader
    {
    public:
        virtual ~ModelLoader() = default;


    };
} // namespace ArchViz
