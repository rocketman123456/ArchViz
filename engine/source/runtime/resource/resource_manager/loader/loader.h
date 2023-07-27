#pragma once

#include <string>

namespace ArchViz
{
    class Loader
    {
    public:
        virtual ~Loader() = default;

        virtual void loadFromFile(const std::string& uri) = 0;
        virtual void unload(const std::string& uri)       = 0;
    };
} // namespace ArchViz
