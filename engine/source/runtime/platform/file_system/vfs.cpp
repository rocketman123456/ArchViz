#include "runtime/platform/file_system/vfs.h"

#include "runtime/platform/file_system/basic/file_utils.h"
#include "runtime/platform/file_system/memory_file/memory_file.h"
#include "runtime/platform/file_system/memory_file/memory_file_system.h"
#include "runtime/platform/file_system/native_file/native_file.h"
#include "runtime/platform/file_system/native_file/native_file_system.h"
#include "runtime/platform/file_system/zip_file/zip_file.h"
#include "runtime/platform/file_system/zip_file/zip_file_system.h"

// #include "runtime/function/global/global_context.h"
// #include "runtime/resource/config_manager/config_manager.h"

#include <algorithm>

namespace ArchViz
{
    void VFS::mount(const VFSConfig& config)
    {
        for (auto& fs : config.m_configs)
        {
            mountFS(fs);
        }
        buildVFSCache();
    }

    void VFS::mountFS(const FSConfig& fs)
    {
        if (fs.m_type == "native")
        {
            auto root  = std::string(""); // g_runtime_global_context.m_config_manager->getRootFolder().string();
            auto rpath = combine_path(root, fs.m_rpath);
            m_fs.emplace_back(std::make_shared<NativeFileSystem>(fs.m_vpath, rpath, fs));
        }
        else if (fs.m_type == "memory")
        {
            m_fs.emplace_back(std::make_shared<MemoryFileSystem>(fs.m_vpath, fs.m_rpath, fs));
        }
        else if (fs.m_type == "compress-zip")
        {
            auto root  = std::string(""); // g_runtime_global_context.m_config_manager->getRootFolder().string();
            auto rpath = combine_path(root, fs.m_rpath);
            m_fs.emplace_back(std::make_shared<ZipFileSystem>(fs.m_vpath, rpath, fs));
        }
    }

    void VFS::unmountFS(const FSConfig& config)
    {
        auto iter = std::find_if(m_fs.begin(), m_fs.end(), [config](FileSystemPtr fs) { return fs->m_rpath == config.m_rpath && fs->m_vpath == config.m_vpath; });
        if (iter != m_fs.end())
        {
            m_fs.erase(iter);
        }
    }

    void VFS::unmountAll()
    {
        m_fs.clear();
        m_fileCache.clear();
        m_dirCache.clear();
    }

    void VFS::buildVFSCache()
    {
        for (auto fs : m_fs)
        {
            fs->buildFSCache();

            for (auto file : fs->m_vfiles)
            {
                m_fileCache[file] = fs;
            }
            for (auto dir : fs->m_vdirs)
            {
                m_dirCache[dir] = fs;
            }
        }
    }

    FilePtr VFS::open(const std::string& vpath, uint32_t mode)
    {
        auto fs = m_fileCache.find(vpath);
        if (fs == m_fileCache.end())
        {
            return nullptr;
        }
        else
        {
            return fs->second->open(vpath, mode);
        }
    }

    bool VFS::close(FilePtr file) { return file->close(); }

    size_t VFS::read(FilePtr file, std::vector<std::byte>& buffer) { return file->read(buffer); }

    size_t VFS::write(FilePtr file, const std::vector<std::byte>& buffer) { return file->write(buffer); }

    std::future<size_t> VFS::readAsync(std::shared_ptr<WorkExecutor> tp, FilePtr file, std::vector<std::byte>& buffer) { return tp->enqueue_task(&VFS::read, this, file, buffer); }

    std::future<size_t> VFS::writeAsync(std::shared_ptr<WorkExecutor> tp, FilePtr file, const std::vector<std::byte>& buffer) { return tp->enqueue_task(&VFS::write, this, file, buffer); }
} // namespace ArchViz
