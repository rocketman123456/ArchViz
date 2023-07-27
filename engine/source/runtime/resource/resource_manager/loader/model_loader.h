#pragma once
#include "runtime/resource/resource_manager/loader/loader.h"
#include "runtime/resource/resource_manager/resource_handle.h"

#include <cstdint>
#include <string>

namespace ArchViz
{
    class ModelLoader : public Loader
    {
    public:
        virtual ~ModelLoader() = default;

        //virtual MeshHandle getMesh(const std::string& uri) = 0;
        //virtual MeshHandle getMesh(uint64_t hash_uri)      = 0;

        //virtual MaterialHandle getMateial(const std::string& uri) = 0;
        //virtual MaterialHandle getMateial(uint64_t hash_uri)      = 0;

        virtual ModelHandle getModel(const std::string& uri) = 0;
        virtual ModelHandle getModel(uint64_t hash_uri)      = 0;
    };
} // namespace ArchViz
