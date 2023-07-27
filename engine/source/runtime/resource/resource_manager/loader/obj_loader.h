#pragma once
#include "runtime/resource/resource_manager/loader/model_loader.h"

#include <memory>
#include <string>
#include <vector>

namespace ArchViz
{
    class ResourceManager;

    class ObjLoader : public ModelLoader
    {
    public:
        virtual ~ObjLoader() = default;

        ModelHandle getModel(const std::string& uri) override;
        ModelHandle getModel(uint64_t hash_uri) override;

        void loadFromFile(const std::string& uri) override;
        void unload(const std::string& uri) override;

    public:
        std::weak_ptr<ResourceManager> m_resource_manager;
    };
} // namespace ArchViz
