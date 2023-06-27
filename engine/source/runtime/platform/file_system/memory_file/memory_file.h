#pragma once
#include "runtime/platform/file_system/basic/file.h"

namespace ArchViz
{
    class MemoryFile : public File
    {
    public:
        MemoryFile(const std::string& vpath, const std::string& rpath);
        virtual ~MemoryFile();

        virtual bool open(uint32_t mode) override;
        virtual bool close() override;

        virtual bool   isOpened() const override;
        virtual bool   isReadOnly() const override;
        virtual size_t size() const override;
        virtual size_t seek(size_t offset, Origin origin) override;
        virtual size_t tell() override;
        // If Read Text, will read [size - 1] bytes, because last will be ['\0'] for string
        virtual size_t read(std::vector<std::byte>& data) override;
        virtual size_t write(const std::vector<std::byte>& data) override;
        virtual size_t read(std::string& data) override;
        virtual size_t write(const std::string& data) override;

    private:
        size_t   m_seek_pos     = 0;
        uint32_t m_mode         = 0;
        bool     m_is_read_only = false;
        bool     m_opened       = false;

        FileBuffer m_buffer;
    };

    using MemoryFilePtr = std::shared_ptr<MemoryFile>;
} // namespace ArchViz
