#include "runtime/platform/file_system/memory_file/memory_file_system.h"
#include "runtime/platform/file_system/basic/file_utils.h"

namespace Piccolo
{
    MemoryFileSystem::MemoryFileSystem(const std::string& vpath, const std::string& rpath) : FileSystem(vpath, rpath) {}

    void MemoryFileSystem::buildFSCache() {}

    FilePtr MemoryFileSystem::open(const std::string& vpath_, uint32_t mode)
    {
        auto vpath = get_normalized_path(vpath_);
        return std::make_shared<MemoryFile>(vpath, vpath);
    }

    bool MemoryFileSystem::close(FilePtr file) { return file->close(); }
} // namespace Piccolo
