#pragma once
#include <string>
#include <zip.h>

namespace ArchViz
{
    zip_t*      open_zip(const std::string& file_name, uint32_t mode = ZIP_CREATE);
    void        close_zip(zip_t* zip);
    zip_file_t* open_zip_file(zip_t* zip, const std::string& file_name, uint32_t mode = 0);
    void        close_zip_file(zip_file_t* zip_file);
    void        get_zip_file_status(zip_t* zip, zip_stat_t* status, const std::string& file_name, uint32_t mode = 0);
} // namespace ArchViz
