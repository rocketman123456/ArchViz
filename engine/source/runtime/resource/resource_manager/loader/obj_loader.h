#pragma once
#include "runtime/resource/resource_manager/loader/loader.h"

#include "runtime/resource/res_type/components/mesh_res.h"
#include "runtime/resource/res_type/data/mesh_data.h"

#include <tiny_obj_loader.h>

#include <memory>
#include <string>
#include <vector>

namespace ArchViz
{
    class ObjLoader : public Loader<MeshData, SubMeshRes>
    {
    public:
        virtual ~ObjLoader() = default;

        std::pair<std::shared_ptr<MeshData>, size_t> createResource(const SubMeshRes& create_info) override;
        std::pair<std::shared_ptr<MeshData>, size_t> createResource(const std::string& uri) override;

    private:
        std::shared_ptr<MeshData> convertMeshData(const tinyobj::attrib_t& attrib, const std::vector<tinyobj::shape_t>& shapes, const std::vector<tinyobj::material_t>& materials);
    };
} // namespace ArchViz
