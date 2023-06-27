#include "runtime/platform/file_system/memory_file/memory_file.h"

#include "runtime/core/base/macro.h"

#include <utility>

namespace ArchViz
{
    MemoryFile::MemoryFile(const std::string& vpath, const std::string& rpath) : File(vpath, rpath) {}

    MemoryFile::~MemoryFile() { close(); }

    bool MemoryFile::open(uint32_t mode)
    {
        // Check Re-open case
        if (isOpened() && (this->m_mode & mode) != 0)
        {
            LOG_WARN("Reopen Memory File {}, {}", m_vpath, m_rpath);
            seek(0, File::beg);
            return true;
        }
        this->m_mode         = mode;
        this->m_seek_pos     = 0;
        this->m_is_read_only = true;
        if (mode & File::write_bin || mode & File::write_text)
        {
            m_is_read_only = false;
        }
        if (mode & File::append)
        {
            m_is_read_only = false;
            m_seek_pos     = size() > 0 ? size() - 1 : 0;
        }
        if (mode & File::truncate)
        {
            m_is_read_only = false;
            m_buffer.clear();
        }
        m_opened = true;
        return true;
    }

    bool MemoryFile::close()
    {
        m_buffer.clear();
        m_is_read_only = true;
        m_opened       = false;
        m_seek_pos     = 0;
        return true;
    }

    bool MemoryFile::isOpened() const { return m_opened; }

    bool MemoryFile::isReadOnly() const { return m_is_read_only; }

    size_t MemoryFile::size() const { return m_buffer.size(); }

    size_t MemoryFile::seek(size_t offset, Origin origin)
    {
        if (!isOpened())
        {
            LOG_WARN("Seek file not opened {}, {}", m_vpath, m_rpath);
            return std::size_t(0);
        }

        if (origin == File::beg)
            m_seek_pos = offset;
        else if (origin == File::end)
            m_seek_pos = size() - offset;
        else
            m_seek_pos += offset;

        m_seek_pos = std::max(m_seek_pos, std::size_t(0));
        m_seek_pos = std::min(m_seek_pos, size() - 1);
        return tell();
    }

    size_t MemoryFile::tell() { return m_seek_pos; }

    size_t MemoryFile::read(std::vector<std::byte>& data)
    {
        if (m_mode & File::read_text)
        {
            data.resize(m_buffer.size() + 1);
            std::copy(m_buffer.begin(), m_buffer.end(), data.begin());
            data[data.size() - 1] = std::byte('\0');
            return data.size();
        }
        else if (m_mode & File::read_bin)
        {
            data.resize(m_buffer.size());
            std::copy(m_buffer.begin(), m_buffer.end(), data.begin());
            return data.size();
        }
        else
        {
            return 0;
        }
    }

    size_t MemoryFile::write(const std::vector<std::byte>& data)
    {
        if (m_mode & File::write_text)
        {
            m_buffer.resize(data.size() + 1);
            std::copy(data.begin(), data.end(), m_buffer.begin());
            m_buffer[m_buffer.size() - 1] = std::byte('\0');
            return m_buffer.size();
        }
        else if (m_mode & File::write_bin)
        {
            m_buffer.resize(data.size());
            std::copy(data.begin(), data.end(), m_buffer.begin());
            return m_buffer.size();
        }
        else
        {
            return 0;
        }
    }

    size_t MemoryFile::read(std::string& data)
    {
        if (m_mode & File::read_text)
        {
            data.resize(m_buffer.size() + 1);
            std::memcpy(data.data(), m_buffer.data(), m_buffer.size());
            data.data()[data.size() - 1] = '\0';
            return data.size();
        }
        else if (m_mode & File::read_bin)
        {
            data.resize(m_buffer.size());
            std::memcpy(data.data(), m_buffer.data(), m_buffer.size());
            return data.size();
        }
        else
        {
            return 0;
        }
    }

    size_t MemoryFile::write(const std::string& data)
    {
        if (m_mode & File::read_text)
        {
            m_buffer.resize(data.size() + 1);
            std::memcpy(m_buffer.data(), data.data(), m_buffer.size());
            m_buffer[m_buffer.size() - 1] = std::byte('\0');
            return m_buffer.size();
        }
        else if (m_mode & File::read_bin)
        {
            m_buffer.resize(data.size());
            std::memcpy(m_buffer.data(), data.data(), m_buffer.size());
            return m_buffer.size();
        }
        else
        {
            return 0;
        }
    }
} // namespace ArchViz
