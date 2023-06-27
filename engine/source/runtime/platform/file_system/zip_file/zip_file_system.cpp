#include "runtime/platform/file_system/zip_file/zip_file_system.h"
#include "runtime/platform/file_system/basic/file_utils.h"
#include "runtime/platform/file_system/zip_file/zip_utils.h"

#include "runtime/core/base/macro.h"
#include "runtime/core/string/string_utils.h"

#include <exception>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace ArchViz
{
    ZipFileSystem::ZipFileSystem(const std::string& vpath, const std::string& rpath, const FSConfig& config) : FileSystem(vpath, rpath, config)
    {
        // normalize path
        m_vpath = get_normalized_path(vpath);
        // check zip exist
        std::filesystem::path basic = rpath;
        if (!std::filesystem::is_regular_file(basic))
        {
            LOG_ERROR("Zip File System {} Not Exist", rpath);
            throw std::invalid_argument("Zip File System Not Exist");
        }
        // initialize zip file
        m_zip = open_zip(rpath, ZIP_RDONLY); // ZIP_CREATE
        if (m_zip == nullptr)
        {
            throw std::runtime_error("Unable to Open Zip");
        }
    }

    ZipFileSystem::~ZipFileSystem()
    {
        if (m_zip != nullptr)
        {
            close_zip(m_zip);
        }
    }

    void ZipFileSystem::buildFSCache()
    {
        zip_int64_t num_entries = zip_get_num_entries(m_zip, 0);
        for (zip_int64_t i = 0; i < num_entries; ++i)
        {
            const char* name = zip_get_name(m_zip, i, 0);
            // LOG_DEBUG("zip: {}", name);
            if (end_with(name, "/"))
            {
                std::string temp = name;

                temp = temp.substr(0, temp.size() - 1);
                m_rdirs.push_back(temp);

                temp = combine_path(m_vpath, temp);
                m_vdirs.push_back(temp);
            }
            else
            {
                std::string temp = name;
                m_rfiles.push_back(temp);

                temp = combine_path(m_vpath, temp);
                m_vfiles.push_back(temp);
            }
        }
    }

    FilePtr ZipFileSystem::open(const std::string& vpath_, uint32_t mode)
    {
        // normalize vpath
        auto vpath = get_normalized_path(vpath_);
        // remove native file system vpath prefix
        std::string temp_vpath = vpath.substr(m_vpath.size() + 1, vpath.size() - m_vpath.size() - 1);
        // get real path
        ZipFilePtr file = std::make_shared<ZipFile>(vpath, temp_vpath, m_zip);
        if (file->open(mode))
            return file;
        else
            return nullptr;
    }

    bool ZipFileSystem::close(FilePtr file) { return file->close(); }
} // namespace ArchViz
