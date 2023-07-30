#pragma once
#include "runtime/resource/resource_manager/loader/loader.h"

#include <memory>

namespace ArchViz
{
    class ResourceManager;

    class TextureLoader : public ILoader
    {
    public:
        virtual ~TextureLoader() = default;

        void loadFromFile(const std::string& uri);

    public:
        std::weak_ptr<ResourceManager> m_resource_manager;
    };
} // namespace ArchViz
