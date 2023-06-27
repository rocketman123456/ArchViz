#include "runtime/platform/file_system/zip_file/zip_utils.h"

#include "runtime/core/base/macro.h"

#include <cassert>

namespace ArchViz
{
    zip_t* open_zip(const std::string& file_name, uint32_t mode)
    {
        auto zip_archive = zip_open(file_name.c_str(), ZIP_CREATE, nullptr);
        if (zip_archive == nullptr)
        {
            LOG_DEBUG("Unable to Open Zip: {}, Error: {}", file_name, zip_strerror(zip_archive));
            return nullptr;
        }
        return zip_archive;
    }

    void close_zip(zip_t* zip)
    {
        auto result = zip_close(zip);
        if (result < 0)
        {
            LOG_WARN("Zip Close Error: {}", zip_strerror(zip));
        }
    }

    zip_file_t* open_zip_file(zip_t* zip, const std::string& file_name, uint32_t mode)
    {
        assert(zip != nullptr);
        auto file = zip_fopen(zip, file_name.c_str(), mode);
        if (file == nullptr)
        {
            LOG_WARN("Open File Not Exist in Zip: {}", file_name);
        }
        return file;
    }

    void close_zip_file(zip_file_t* zip_file) { zip_fclose(zip_file); }

    void get_zip_file_status(zip_t* zip, zip_stat_t* status, const std::string& file_name, uint32_t mode)
    {
        assert(zip != nullptr && status != nullptr);
        zip_stat_init(status);
        auto result = zip_stat(zip, file_name.c_str(), mode, status);
        if (result < 0)
        {
            LOG_WARN("Get File Info in Zip Error: {}", file_name);
        }
    }
} // namespace ArchViz
