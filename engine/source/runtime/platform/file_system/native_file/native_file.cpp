#include "runtime/platform/file_system/native_file/native_file.h"

#include "runtime/core/base/macro.h"

#include <filesystem>
#include <utility>

namespace ArchViz
{
    bool is_native_readonly(const std::string& real_path)
    {
        auto perm = std::filesystem::status(real_path).permissions();
        if ((perm & std::filesystem::perms::owner_write) == std::filesystem::perms::none)
        {
            return true;
        }
        return false;
    }

    size_t get_native_file_size(const std::string& real_path)
    {
        auto err      = std::error_code {};
        auto filesize = std::filesystem::file_size(real_path, err);
        if (filesize != static_cast<uintmax_t>(-1))
            return filesize;
        else
            return 0;
    }

    NativeFile::NativeFile(const std::string& vpath, const std::string& rpath) : File(vpath, rpath) {}

    bool NativeFile::open(uint32_t mode)
    {
        if (isOpened() && (m_mode & mode) != 0)
        {
            LOG_WARN("Reopen Native File {}, {}", m_vpath, m_rpath);
            seek(0, File::beg);
            return true;
        }
        m_mode = mode;

        std::ios_base::openmode open_mode = (std::ios_base::openmode)0x00;
        if (mode & File::read_bin)
        {
            m_read_only = true;
            open_mode |= std::fstream::in;
            open_mode |= std::fstream::binary;
        }
        if (mode & File::write_bin)
        {
            m_read_only = false;
            open_mode |= std::fstream::out;
            open_mode |= std::fstream::binary;
        }
        if (mode & File::read_text)
        {
            m_read_only = true;
            open_mode |= std::fstream::in;
        }
        if (mode & File::write_text)
        {
            m_read_only = false;
            open_mode |= std::fstream::out;
        }
        if (mode & File::append)
        {
            m_read_only = false;
            open_mode |= std::fstream::app;
        }
        if (mode & File::truncate)
        {
            m_read_only = false;
            open_mode |= std::fstream::trunc;
        }
        // Check in filesystem to assure is_read_only
        if (!m_read_only)
        {
            auto result = is_native_readonly(m_rpath);
            if (result)
            {
                m_read_only = true;
            }
        }
        LOG_DEBUG("Native Path: {}, {}", m_vpath, m_rpath);
        m_stream.open(m_rpath, open_mode);
        return m_stream.is_open();
    }

    bool NativeFile::close()
    {
        m_stream.close();
        return true;
    }

    bool NativeFile::isOpened() const { return m_stream.is_open(); }

    bool NativeFile::isReadOnly() const { return m_read_only; }

    size_t NativeFile::size() const { return get_native_file_size(m_rpath); }

    size_t NativeFile::seek(size_t offset, Origin origin)
    {
        if (!isOpened())
        {
            LOG_WARN("Seek file not opened {}, {}", m_vpath, m_rpath);
            return size_t(0);
        }

        std::ios_base::seekdir way;
        if (origin == File::beg)
            way = std::ios_base::beg;
        else if (origin == File::end)
            way = std::ios_base::end;
        else
            way = std::ios_base::cur;

        m_stream.seekg(offset, way);
        m_stream.seekp(offset, way);
        return tell();
    }

    size_t NativeFile::tell()
    {
        if (!isOpened())
            return size_t(0);
        else
            return static_cast<size_t>(m_stream.tellg());
    }

    size_t NativeFile::read(std::vector<std::byte>& buffer)
    {
        if (!isOpened())
        {
            LOG_WARN("Read file not opened {}, {}", m_vpath, m_rpath);
            return size_t(0);
        }

        size_t buffer_size = size() - tell();
        buffer.clear();
        if (m_mode & File::read_text)
            buffer.resize(buffer_size + 1);
        else
            buffer.resize(buffer_size);

        m_stream.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(buffer_size));

        size_t read_count = 0;
        if (m_stream)
            read_count = buffer_size;
        else
            read_count = static_cast<size_t>(m_stream.gcount());

        if (m_mode & File::read_text)
        {
            buffer.data()[read_count] = std::byte(0);
        }
        return read_count;
    }

    size_t NativeFile::write(const std::vector<std::byte>& data)
    {
        if (!isOpened() || isReadOnly())
        {
            LOG_WARN("Write to Read Only File {}, {}", m_vpath, m_rpath);
            return 0;
        }
        m_stream.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
        if (m_stream)
        {
            return data.size();
        }
        return static_cast<size_t>(m_stream.gcount());
    }

    size_t NativeFile::read(std::string& data)
    {
        if (!isOpened())
        {
            LOG_WARN("Read file not opened {}, {}", m_vpath, m_rpath);
            return size_t(0);
        }
        data = {std::istreambuf_iterator<char>(m_stream), std::istreambuf_iterator<char>()};
        return 0;
    }

    size_t NativeFile::write(const std::string& data)
    {
        if (!isOpened() || isReadOnly())
        {
            LOG_WARN("Write to Read Only File {}, {}", m_vpath, m_rpath);
            return 0;
        }
        m_stream.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size() + 1));
        if (m_stream)
        {
            return data.size() + 1;
        }
        return static_cast<size_t>(m_stream.gcount());
    }

} // namespace ArchViz
