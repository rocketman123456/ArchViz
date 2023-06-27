#pragma once

#include <string>
#include <vector>

namespace ArchViz
{
    void split_multi_char(const std::string& in, const std::string& token, std::vector<std::string>& out);
    void split_single_char(const std::string& in, const char token, std::vector<std::string>& out);
    void split_last_single_char(const std::string& name, const char token, std::string& first, std::string& second);

    std::string replace_all(const std::string& target, const std::string& from, const std::string& to);

    // find matchness, 0 is total unmatch, 1 is total match
    float string_match(const std::string& left, const std::string& right);

    bool start_with(const std::string& full_string, const std::string& starting);
    bool end_with(const std::string& full_string, const std::string& ending);
} // namespace ArchViz
