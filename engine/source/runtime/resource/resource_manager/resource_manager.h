// https://floooh.github.io/2018/06/17/handles-vs-pointers.html
// https://giordi91.github.io/post/resourcesystem/
#pragma once

#include "runtime/resource/res_type/components/material_res.h"
#include "runtime/resource/res_type/components/mesh_res.h"
#include "runtime/resource/res_type/data/material_data.h"
#include "runtime/resource/res_type/data/mesh_data.h"

#include "runtime/resource/resource_manager/loader/loader.h"
#include "runtime/resource/resource_manager/loader/obj_loader.h"
#include "runtime/resource/resource_manager/loader/texture_loader.h"

#include "runtime/resource/resource_manager/resource_handle.h"

#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>

namespace ArchViz
{
    class ResourceManager : public std::enable_shared_from_this<ResourceManager>
    {
    public:
        void initialize();
        void clear();

        void createMesh(const MeshComponentRes& res);
        void createMaterial(const MaterialRes& res);

        std::weak_ptr<MeshData>    getMeshData(const std::string& uri);
        std::weak_ptr<TextureData> getTextureData(const std::string& uri);

        template<typename T>
        void setRawData(const std::string& uri, std::unique_ptr<T> data)
        {
            const std::type_info& type = typeid(T);
        }

        template<typename T>
        std::weak_ptr<T> getRawData(const std::string& uri)
        {
            const std::type_info& type = typeid(T);
            return nullptr;
        }

    private:
        // std::unordered_map<std::string, std::shared_ptr<Loader>> m_loaders;

        std::shared_ptr<ObjLoader>     m_obj_loader;
        std::shared_ptr<TextureLoader> m_texture_loader;

        std::unordered_map<std::string, std::shared_ptr<MeshData>>    m_mesh_data_cache;
        std::unordered_map<std::string, std::shared_ptr<TextureData>> m_texture_cache;
    };
} // namespace ArchViz
