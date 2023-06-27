#include "runtime/core/string/string_utils.h"

namespace ArchViz
{
    void split_multi_char(const std::string& in, const std::string& token, std::vector<std::string>& out)
    {
        out.clear();
        std::string temp;
        for (int i = 0; i < int(in.size()); i++)
        {
            std::string test = in.substr(i, token.size());
            if (test == token)
            {
                if (!temp.empty())
                {
                    out.push_back(temp);
                    temp.clear();
                    i += (int)token.size() - 1;
                }
                // else
                // {
                //     out.push_back("");
                // }
            }
            else if (i + token.size() >= in.size())
            {
                temp += in.substr(i, token.size());
                out.push_back(temp);
                break;
            }
            else
            {
                temp += in[i];
            }
        }
        // Remove Empty String
        for (auto it = out.begin(); it != out.end();)
        {
            if ((*it).length() == 0)
                it = out.erase(it);
            else
                ++it;
        }
    }

    void split_single_char(const std::string& in, const char token, std::vector<std::string>& out)
    {
        out.clear();
        std::size_t start = 0;
        std::size_t end   = 0;
        while ((end = in.find(token, start)) != std::string::npos)
        {
            out.push_back(in.substr(start, end - start));
            start = end + 1;
        }
        out.push_back(in.substr(start));
        // Remove Empty String
        for (auto it = out.begin(); it != out.end();)
        {
            if ((*it).length() == 0)
                it = out.erase(it);
            else
                ++it;
        }
    }

    void split_last_single_char(const std::string& name, const char token, std::string& first, std::string& second)
    {
        std::size_t found = name.rfind(token);
        if (found != std::string::npos)
        {
            first  = name.substr(0, found);
            second = name.substr(found + 1, name.length() - found - 1);
        }
        else
        {
            first  = "";
            second = name;
        }
    }

    std::string replace_all(const std::string& target, const std::string& from, const std::string& to)
    {
        std::string copy = target;
        std::size_t pos  = 0;
        while ((pos = copy.find(from, pos)) != std::string::npos)
        {
            copy.replace(pos, from.length(), to);
            pos += to.length();
        }
        return copy;
    }

    float string_match(const std::string& left, const std::string& right)
    {
        std::size_t left_size        = left.length();
        std::size_t right_size       = right.length();
        std::size_t larger_size      = left_size > right_size ? left_size : right_size;
        std::size_t left_ptr         = 0;
        std::size_t right_ptr        = 0;
        const float CAP_MISMATCH_VAL = 0.9f;
        float       match_val        = 0.0f;

        while (left_ptr < left_size && right_ptr < right_size)
        {
            if (left.at(left_ptr) == right.at(right_ptr))
            {
                match_val += 1.0f / (float)larger_size;
                if (left_ptr < left_size)
                    ++left_ptr;
                if (right_ptr < right_size)
                    ++right_ptr;
            }
            else if (std::tolower(left.at(left_ptr)) == std::tolower(right.at(right_ptr)))
            {
                match_val += CAP_MISMATCH_VAL / (float)larger_size;
                if (left_ptr < left_size)
                    ++left_ptr;
                if (right_ptr < right_size)
                    ++right_ptr;
            }
            else
            {
                std::size_t lpbest      = left_size;
                std::size_t rpbest      = right_size;
                std::size_t total_count = 0;
                std::size_t best_count  = INT_MAX;
                std::size_t left_count  = 0;
                std::size_t right_count = 0;
                for (auto lp = left_ptr; (lp < left_size) && ((left_count + right_count) < best_count); ++lp)
                {
                    for (auto rp = right_ptr; (rp < right_size) && ((left_count + right_count) < best_count); ++rp)
                    {
                        if (std::tolower(left.at(lp)) == std::tolower(right.at(rp)))
                        {
                            total_count = left_count + right_count;
                            if (total_count < best_count)
                            {
                                best_count = total_count;
                                lpbest     = lp;
                                rpbest     = rp;
                            }
                        }
                        ++right_count;
                    }
                    ++left_count;
                    right_count = 0;
                }
                left_ptr  = lpbest;
                right_ptr = rpbest;
            }
        }
        if (match_val > 0.999)
        {
            match_val = 1.0f;
        }
        else if (match_val < 0.001)
        {
            match_val = 0.0f;
        }
        return match_val;
    }

    bool start_with(const std::string& full_string, const std::string& starting)
    {
        if (full_string.length() >= starting.length())
        {
            return (0 == full_string.compare(0, starting.length(), starting));
        }
        else
        {
            return false;
        }
    }

    bool end_with(const std::string& full_string, const std::string& ending)
    {
        if (full_string.length() >= ending.length())
        {
            return (0 == full_string.compare(full_string.length() - ending.length(), ending.length(), ending));
        }
        else
        {
            return false;
        }
    }
} // namespace ArchViz