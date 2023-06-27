#include "runtime/platform/file_system/zip_file/zip_file.h"
#include "runtime/platform/file_system/zip_file/zip_utils.h"

#include "runtime/core/base/macro.h"

#include <filesystem>

namespace ArchViz
{
    ZipFile::ZipFile(const std::string& vpath, const std::string& rpath, zip_t* zip) : File(vpath, rpath), m_zip(zip) {}

    ZipFile::~ZipFile()
    {
        if (m_zip_file != nullptr)
        {
            close();
        }
    }

    bool ZipFile::open(uint32_t mode)
    {
        if (isOpened() && (m_mode & mode) != 0)
        {
            LOG_WARN("Reopen Native File {}, {}", m_vpath, m_rpath);
            return true;
        }
        this->m_mode = mode;

        m_read_only = true; // actually, will be in read only mode
        if (m_mode & File::write_bin || m_mode & File::write_text)
        {
            m_read_only = false;
        }
        if (m_mode & File::append)
        {
            m_read_only = false;
        }
        if (m_mode & File::truncate)
        {
            m_read_only = false;
        }

        if (!m_read_only)
        {
            auto perm = std::filesystem::status(m_rpath).permissions();
            // Check Owner's Permission
            if ((perm & std::filesystem::perms::owner_write) == std::filesystem::perms::none)
            {
                m_read_only = true;
            }
        }

        m_zip_file = open_zip_file(m_zip, m_rpath);
        if (m_zip_file == nullptr)
        {
            LOG_ERROR("Fail to open zip file: {}", m_rpath);
            return false;
        }
        else
        {
            m_opened = true;
            get_zip_file_status(m_zip, &m_zip_file_status, m_rpath);
            LOG_DEBUG("Name: {}", m_zip_file_status.name);
            LOG_DEBUG("\tIndex: {}", m_zip_file_status.index);
            LOG_DEBUG("\tCompressed Size: {}", m_zip_file_status.comp_size);
            LOG_DEBUG("\tSize: {}", m_zip_file_status.size);
            LOG_DEBUG("\tCRC: {}", m_zip_file_status.crc);
            return true;
        }
    }

    bool ZipFile::close()
    {
        if (!isOpened())
            return true;
        close_zip_file(m_zip_file);
        m_zip_file = nullptr;
        m_opened   = false;
        return true;
    }

    bool ZipFile::isOpened() const { return m_opened; }

    bool ZipFile::isReadOnly() const { return m_read_only; }

    size_t ZipFile::size() const { return m_zip_file_status.size; }

    size_t ZipFile::seek(size_t offset, Origin origin)
    {
        LOG_WARN("Unable to Seek in Zip File, {} {}", m_rpath, m_vpath);
        return 0;
    }

    size_t ZipFile::tell()
    {
        LOG_WARN("Unable to Tell in Zip File, {} {}", m_rpath, m_vpath);
        return 0;
    }

    size_t ZipFile::read(std::vector<std::byte>& data)
    {
        if (!isOpened())
        {
            LOG_WARN("Read file not opened {}, {}", m_vpath, m_rpath);
            return std::size_t(0);
        }

        std::size_t read_size = size();
        if (m_mode & File::read_text)
        {
            data.resize(read_size + 1);
        }
        else
        {
            data.resize(read_size);
        }

        zip_fread(m_zip_file, data.data(), read_size);

        if (m_mode & File::read_text)
        {
            data[read_size] = std::byte(0);
        }
        return read_size;
    }

    size_t ZipFile::write(const std::vector<std::byte>& data)
    {
        LOG_WARN("Unable to Weite in Zip File, {} {}", m_rpath, m_vpath);
        return 0;
    }

    size_t ZipFile::read(std::string& data)
    {
        if (!isOpened())
        {
            LOG_WARN("Read file not opened {}, {}", m_vpath, m_rpath);
            return std::size_t(0);
        }

        std::size_t read_size = size();
        if (m_mode & File::read_text)
        {
            data.resize(read_size + 1);
        }
        else
        {
            data.resize(read_size);
        }

        zip_fread(m_zip_file, data.data(), read_size);

        if (m_mode & File::read_text)
        {
            data[read_size] = '\0';
        }
        return read_size;
    }

    size_t ZipFile::write(const std::string& data)
    {
        LOG_WARN("Unable to Weite in Zip File, {} {}", m_rpath, m_vpath);
        return 0;
    }
} // namespace ArchViz
