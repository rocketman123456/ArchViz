#pragma once
#include "runtime/platform/file_system/basic/file_system.h"
#include "runtime/platform/file_system/zip_file/zip_file.h"

#include <zip.h>

namespace Piccolo
{
    class ZipFileSystem : public FileSystem
    {
    public:
        ZipFileSystem(const std::string& vpath, const std::string& rpath);
        virtual ~ZipFileSystem();

        virtual void buildFSCache() override;

        virtual FilePtr open(const std::string& vpath, uint32_t mode) override;
        virtual bool    close(FilePtr file) override;

    private:
        zip_t* m_zip {nullptr};
    };
}