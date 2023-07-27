#include "runtime/resource/resource_manager/loader/obj_loader.h"

#include "runtime/function/global/global_context.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"
#include "runtime/resource/resource_manager/resource_manager.h"

#include "runtime/resource/res_type/components/material_res.h"
#include "runtime/resource/res_type/components/mesh_res.h"
#include "runtime/resource/res_type/data/material_data.h"
#include "runtime/resource/res_type/data/mesh_data.h"

#include "runtime/core/base/macro.h"

#include <tiny_obj_loader.h>

#include <filesystem>

namespace ArchViz
{
    ModelHandle ObjLoader::getModel(const std::string& uri)
    {
        //
        return {};
    }

    ModelHandle ObjLoader::getModel(uint64_t hash_uri)
    {
        //
        return {};
    }

    void ObjLoader::loadFromFile(const std::string& uri)
    {
        std::filesystem::path model_uri = g_runtime_global_context.m_config_manager->getRootFolder() / uri;

        tinyobj::attrib_t                attrib;
        std::vector<tinyobj::shape_t>    shapes;
        std::vector<tinyobj::material_t> materials;
        std::string                      warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, model_uri.generic_string().c_str(), model_uri.parent_path().generic_string().c_str()))
        {
            LOG_WARN(warn);
            LOG_FATAL(err);
        }

        if (!warn.empty())
        {
            LOG_WARN(warn);
        }

        std::unique_ptr<MeshData> mesh;

        // std::unordered_map<Vertex, uint32_t> unique_vertices {};

        // for (const auto& shape : shapes)
        //{
        //     for (const auto& index : shape.mesh.indices)
        //     {
        //         Vertex vertex {};

        //        vertex.pos = {attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1], attrib.vertices[3 * index.vertex_index + 2]};
        //        // vertex.color = {attrib.colors[3 * index.vertex_index + 0], attrib.colors[3 * index.vertex_index + 1], attrib.colors[3 * index.vertex_index + 2]};
        //        vertex.color = {1.0f, 1.0f, 1.0f};
        //        vertex.normal = {attrib.normals[3 * index.normal_index + 0], attrib.normals[3 * index.normal_index + 1], attrib.normals[3 * index.normal_index + 2]};
        //        vertex.tex_coord = {attrib.texcoords[2 * index.texcoord_index + 0], 1.0f - attrib.texcoords[2 * index.texcoord_index + 1]}; // invert tex y axis
        //        // vertex.tex_coord = {attrib.texcoords[2 * index.texcoord_index + 0], attrib.texcoords[2 * index.texcoord_index + 1]};

        //        if (unique_vertices.count(vertex) == 0)
        //        {
        //            unique_vertices[vertex] = static_cast<uint32_t>(m_vertices.size());
        //            m_vertices.push_back(vertex);
        //        }

        //        m_indices.push_back(unique_vertices[vertex]);
        //    }
        //}
    }

    void ObjLoader::unload(const std::string& uri)
    {
        //
    }
} // namespace ArchViz
