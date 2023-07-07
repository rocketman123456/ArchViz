#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace ArchViz
{
    using FileBuffer = std::vector<std::byte>;

    class FileSystem;

    class File
    {
    public:
        enum Mode : uint32_t
        {
            read_bin       = 1 << 0,
            write_bin      = 1 << 1,
            readwrite_bin  = read_bin | write_bin,
            read_text      = 1 << 2,
            write_text     = 1 << 3,
            readwrite_text = read_text | write_text,
            append         = 1 << 4,
            truncate       = 1 << 5,
        };

        enum Origin : uint32_t
        {
            beg = 0,
            end = 1,
            set = 2,
        };

    public:
        explicit File(const std::string& vpath, const std::string& rpath) : m_vpath {vpath}, m_rpath {rpath} {}
        virtual ~File() = default;

        virtual bool open(uint32_t mode) = 0;
        virtual bool close()             = 0;

        virtual bool   isOpened() const                   = 0;
        virtual bool   isReadOnly() const                 = 0;
        virtual size_t size() const                       = 0;
        virtual size_t seek(size_t offset, Origin origin) = 0;
        virtual size_t tell()                             = 0;
        // If Read Text, will read [size - 1] bytes, because last will be ['\0'] for string
        virtual size_t read(std::vector<std::byte>& data)        = 0;
        virtual size_t write(const std::vector<std::byte>& data) = 0;
        virtual size_t read(std::string& data)                   = 0;
        virtual size_t write(const std::string& data)            = 0;

    public:
        uint32_t m_mode;

        std::string m_vpath;
        std::string m_rpath;

        std::shared_ptr<FileSystem> m_fs;
    };

    using FilePtr = std::shared_ptr<File>;
} // namespace ArchViz
