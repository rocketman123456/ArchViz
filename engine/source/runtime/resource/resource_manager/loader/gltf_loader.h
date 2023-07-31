#pragma once
#include "runtime/resource/resource_manager/loader/model_loader.h"

#include "runtime/resource/res_type/components/mesh_res.h"
#include "runtime/resource/res_type/data/mesh_data.h"

#include <tiny_gltf.h>

#include <memory>
#include <string>
#include <vector>

namespace ArchViz
{
    class glTFLoader : public Loader<MeshData, SubMeshRes>
    {
    public:
        virtual ~glTFLoader() = default;
    };
} // namespace ArchViz
