#pragma once
#include "runtime/platform/file_system/basic/file.h"

#include <zip.h>

namespace ArchViz
{
    class ZipFile : public File
    {
    public:
        ZipFile(const std::string& vpath, const std::string& rpath, zip_t* zip);
        virtual ~ZipFile();

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
        zip_t*      m_zip {nullptr};
        zip_file_t* m_zip_file {nullptr};
        zip_stat_t  m_zip_file_status {};
        uint32_t    m_mode {0};
        bool        m_read_only {true};
        bool        m_opened {false};
    };

    using ZipFilePtr = std::shared_ptr<ZipFile>;
} // namespace ArchViz