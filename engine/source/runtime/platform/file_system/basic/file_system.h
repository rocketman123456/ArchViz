#pragma once
#include "runtime/core/meta/reflection/reflection.h"
#include "runtime/core/thread/thread_pool.h"

#include "runtime/platform/file_system/basic/file.h"

#include <algorithm>
#include <future>

namespace ArchViz
{
    REFLECTION_TYPE(FSConfig)
    CLASS(FSConfig, Fields)
    {
        REFLECTION_BODY(FSConfig)
    public:
        std::string m_vpath;
        std::string m_rpath;
        std::string m_type;
        // TODO : add ignore sub dir support
        std::vector<std::string> m_ignores;
    };

    class FileSystem
    {
    public:
        FileSystem(const std::string& vpath, const std::string& rpath, const FSConfig& config) : m_vpath {vpath}, m_rpath {rpath}, m_config(config) {}
        virtual ~FileSystem() = default;

        bool isFileExist(const std::string& file_name) const
        {
            auto iter = std::find_if(m_vfiles.begin(), m_vfiles.end(), [file_name](const std::string& file) { return file_name == file; });
            return iter != m_vfiles.end();
        }

        bool isDirExist(const std::string& dir_name) const
        {
            auto iter = std::find_if(m_vdirs.begin(), m_vdirs.end(), [dir_name](const std::string& file) { return dir_name == file; });
            return iter != m_vdirs.end();
        }

        size_t read(FilePtr file, std::vector<std::byte>& buffer) { return file->read(buffer); }
        size_t write(FilePtr file, const std::vector<std::byte>& buffer) { return file->write(buffer); }
        size_t read(FilePtr file, std::string& buffer) { return file->read(buffer); }
        size_t write(FilePtr file, const std::string& buffer) { return file->write(buffer); }

        std::future<size_t> readAsync(std::shared_ptr<WorkExecutor> tp, FilePtr file, std::vector<std::byte>& buffer)
        {
            return tp->enqueue_task([file, &buffer]() { return file->read(buffer); });
        }

        std::future<size_t> writeAsync(std::shared_ptr<WorkExecutor> tp, FilePtr file, const std::vector<std::byte>& buffer)
        {
            return tp->enqueue_task([file, &buffer]() { return file->write(buffer); });
        }

        std::future<size_t> readAsync(std::shared_ptr<WorkExecutor> tp, FilePtr file, std::string& buffer)
        {
            return tp->enqueue_task([file, &buffer]() { return file->read(buffer); });
        }

        std::future<size_t> writeAsync(std::shared_ptr<WorkExecutor> tp, FilePtr file, const std::string& buffer)
        {
            return tp->enqueue_task([file, &buffer]() { return file->write(buffer); });
        }

        // TODO
        // virtual bool createFile(const std::string& file_path) = 0;
        // virtual bool deleteFile(const std::string& file_path) = 0;
        // virtual bool moveFile(const std::string& src, const std::string& dst) = 0;
        // virtual bool copyFile(const std::string& src, const std::string& dst) = 0;

        // TODO
        // virtual bool createDir(const std::string& dir_path) = 0;
        // virtual bool deleteDir(const std::string& dir_path) = 0;
        // virtual bool moveDir(const std::string& src, const std::string& dst) = 0;
        // virtual bool copyDir(const std::string& src, const std::string& dst) = 0;

        // -------------------------------------------------------------------
        // -------------------------------------------------------------------
        // -------------------------------------------------------------------

        virtual void    buildFSCache()                                = 0;
        virtual FilePtr open(const std::string& vpath, uint32_t mode) = 0;
        virtual bool    close(FilePtr file)                           = 0;

        // -------------------------------------------------------------------
        // -------------------------------------------------------------------
        // -------------------------------------------------------------------

    public:
        std::string m_vpath;
        std::string m_rpath;

        FSConfig m_config;

        std::vector<std::string> m_vfiles;
        std::vector<std::string> m_vdirs;

        std::vector<std::string> m_rfiles;
        std::vector<std::string> m_rdirs;
    };

    using FileSystemPtr = std::shared_ptr<FileSystem>;
} // namespace ArchViz
