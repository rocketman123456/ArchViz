#pragma once
#include "runtime/platform/file_system/basic/file_system.h"

namespace ArchViz
{
    class NativeFileSystem : public FileSystem
    {
    public:
        NativeFileSystem(const std::string& vpath, const std::string& rpath, const FSConfig& config);
        virtual ~NativeFileSystem() = default;

        virtual void buildFSCache() override;

        virtual FilePtr open(const std::string& vpath, uint32_t mode) override;
        virtual bool    close(FilePtr file) override;
    };
} // namespace ArchViz