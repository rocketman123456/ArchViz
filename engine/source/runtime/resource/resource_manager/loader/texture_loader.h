#pragma once
#include "runtime/resource/resource_manager/loader/loader.h"

#include <memory>

namespace ArchViz
{
    class ResourceManager;

    class TextureLoader : public Loader
    {
    public:
        virtual ~TextureLoader() = default;

        void loadFromFile(const std::string& uri) override;
        void unload(const std::string& uri) override;

    public:
        std::weak_ptr<ResourceManager> m_resource_manager;
    };
} // namespace ArchViz
