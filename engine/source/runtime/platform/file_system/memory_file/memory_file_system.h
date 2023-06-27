#pragma once
#include "runtime/platform/file_system/basic/file_system.h"
#include "runtime/platform/file_system/memory_file/memory_file.h"

namespace ArchViz
{
    class MemoryFileSystem : public FileSystem
    {
    public:
        MemoryFileSystem(const std::string& vpath, const std::string& rpath, const FSConfig& config);
        virtual ~MemoryFileSystem() = default;

        virtual void buildFSCache() override;

        virtual FilePtr open(const std::string& vpath, uint32_t mode) override;
        virtual bool    close(FilePtr file) override;
    };
} // namespace ArchViz