#pragma once
#include "runtime/platform/file_system/basic/file.h"
#include "runtime/platform/file_system/basic/file_system.h"
#include "runtime/platform/file_system/vfs_config.h"

// #include "runtime/platform/file_system/memory_file/memory_file.h"
// #include "runtime/platform/file_system/memory_file/memory_file_system.h"
// #include "runtime/platform/file_system/native_file/native_file.h"
// #include "runtime/platform/file_system/native_file/native_file_system.h"
// #include "runtime/platform/file_system/zip_file/zip_file.h"
// #include "runtime/platform/file_system/zip_file/zip_file_system.h"

#include <unordered_map>

namespace Piccolo
{
    class VFS
    {
    public:
        void mount(const VFSConfig& config);
        void unmountAll();

        FilePtr open(const std::string& vpath, uint32_t mode);
        bool    close(FilePtr file);

        size_t read(FilePtr file, std::vector<std::byte>& buffer);
        size_t write(FilePtr file, const std::vector<std::byte>& buffer);

        std::future<size_t> readAsync(std::shared_ptr<ThreadPool> tp, FilePtr file, std::vector<std::byte>& buffer);
        std::future<size_t> writeAsync(std::shared_ptr<ThreadPool> tp, FilePtr file, const std::vector<std::byte>& buffer);

    private:
        void mountFS(const FSConfig& fs);
        void unmountFS(const FSConfig& fs);

        void buildVFSCache();

    private:
        std::vector<FileSystemPtr>                     m_fs;
        std::unordered_map<std::string, FileSystemPtr> m_fileCache;
        std::unordered_map<std::string, FileSystemPtr> m_dirCache;
    };

    extern VFS g_vfs;
} // namespace Piccolo