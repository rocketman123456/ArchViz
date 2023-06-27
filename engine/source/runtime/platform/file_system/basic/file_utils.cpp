#include "runtime/platform/file_system/basic/file_utils.h"

#include "runtime/core/string/string_utils.h"

namespace ArchViz
{
    std::string get_normalized_path(const std::string& path)
    {
        // replace '\\'
        auto temp = replace_all(path, "\\", "/");
        // remove front '/'
        if (temp.size() > 0 && temp.at(0) == '/')
        {
            temp = temp.substr(1, temp.size() - 1);
        }
        return temp;
    }

    std::string combine_path(const std::string& path1, const std::string& path2)
    {
        if (path1.size() == 0)
        {
            return path2;
        }
        else
        {
            return path1 + "/" + path2;
        }
    }
} // namespace ArchViz
