#include "runtime/platform/file_system/native_file/native_file_system.h"
#include "runtime/platform/file_system/basic/file_utils.h"
#include "runtime/platform/file_system/native_file/native_file.h"
#include "runtime/platform/path/path.h"

#include "runtime/core/base/macro.h"
#include "runtime/core/string/string_utils.h"

#include <exception>
#include <filesystem>
#include <stdexcept>

namespace ArchViz
{
    NativeFileSystem::NativeFileSystem(const std::string& vpath, const std::string& rpath, const FSConfig& config) : FileSystem(vpath, rpath, config)
    {
        if (!std::filesystem::exists(rpath))
        {
            LOG_ERROR("Native File System {} Not Exist", rpath);
            throw std::invalid_argument("Native File System Not Exist");
        }
        if (!std::filesystem::is_directory(rpath))
        {
            LOG_ERROR("Native File System {} Not a Dir Path", rpath);
            throw std::invalid_argument("Native File System Not A Path");
        }
    }

    void NativeFileSystem::buildFSCache()
    {
        // iterate all file in current path
        for (auto const& directory_entry : std::filesystem::recursive_directory_iterator {m_rpath})
        {
            if (directory_entry.is_regular_file()) // build file cache
            {
                std::filesystem::path path = directory_entry;

                auto path_str  = path.generic_string(); // string();
                auto vpath_str = Path::getRelativePath(m_rpath, path).string();
                vpath_str      = get_normalized_path(vpath_str);
                vpath_str      = m_vpath + "/" + vpath_str;
                m_rfiles.push_back(path_str);
                m_vfiles.push_back(vpath_str);
            }
            else if (directory_entry.is_directory()) // build dir cache
            {
                std::filesystem::path path = directory_entry;

                auto path_str  = path.generic_string(); // string();
                auto vpath_str = Path::getRelativePath(m_rpath, path).string();
                vpath_str      = get_normalized_path(vpath_str);
                vpath_str      = m_vpath + "/" + vpath_str;
                m_rdirs.push_back(path_str);
                m_vdirs.push_back(vpath_str);
            }
        }
    }

    FilePtr NativeFileSystem::open(const std::string& vpath_, uint32_t mode)
    {
        // normalize vpath
        auto vpath = get_normalized_path(vpath_);
        // remove native file system vpath prefix
        std::string temp_vpath = vpath.substr(m_vpath.size() + 1, vpath.size() - m_vpath.size() - 1);
        // get real path
        std::string   rpath = m_rpath + "/" + temp_vpath;
        NativeFilePtr file  = std::make_shared<NativeFile>(vpath, rpath);
        if (file->open(mode))
            return file;
        else
            return nullptr;
    }

    bool NativeFileSystem::close(FilePtr file) { return file->close(); }
} // namespace ArchViz
