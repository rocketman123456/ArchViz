#pragma once
#include "runtime/platform/file_system/basic/file.h"

#include <fstream>

namespace ArchViz
{
    class NativeFile : public File
    {
    public:
        NativeFile(const std::string& vpath, const std::string& rpath);
        virtual ~NativeFile() = default;

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
        std::fstream m_stream;
        bool         m_read_only {false};
    };

    using NativeFilePtr = std::shared_ptr<NativeFile>;
} // namespace ArchViz