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
    class ResourceManager;

    class ObjLoader : public Loader<MeshData, SubMeshRes>
    {
    public:
        virtual ~ObjLoader() = default;

        std::shared_ptr<MeshData> createResource(const SubMeshRes& create_info) override;
        std::shared_ptr<MeshData> createResource(const std::string& uri) override;

    private:
        std::shared_ptr<MeshData> convertMeshData(const tinyobj::attrib_t& attrib, const std::vector<tinyobj::shape_t>& shapes, const std::vector<tinyobj::material_t>& materials);

    public:
        std::weak_ptr<ResourceManager> m_resource_manager;
    };
} // namespace ArchViz
