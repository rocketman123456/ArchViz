#pragma once

#include <filesystem>
#include <vector>

namespace ArchViz
{
    class FileService
    {
    public:
        std::vector<std::filesystem::path> getFiles(const std::filesystem::path& directory);
    };
} // namespace ArchViz