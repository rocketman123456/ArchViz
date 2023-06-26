#include "runtime/core/base/hash.h"

namespace Piccolo
{
    int32_t hash_1(const std::string& key)
    {
        int32_t hash_ = 5381;
        auto    count = key.size();
        while (count > 0)
        {
            hash_ += (hash_ << 5) + key[key.size() - count];
            count--;
        }
        return (hash_ & 0x7fffffff);
    }

    int32_t hash_2(const std::string& key)
    {
        int32_t     seed  = 131;
        int32_t     hash_ = 0;
        std::string str   = key + "key2";
        auto        count = str.size();
        while (count > 0)
        {
            hash_ = hash_ * seed + str[str.size() - count];
            count--;
        }
        return (hash_ & 0x7fffffff);
    }

    int32_t hash_3(const std::string& key)
    {
        int32_t     hash_ = 0;
        std::string str   = key + "keykey3";
        auto        count = str.size();
        for (int i = 0; i < count; ++i)
        {
            if ((i * 1) == 0)
            {
                hash_ ^= ((hash_ << 7) ^ (str[i] ^ hash_ >> 3));
            }
            else
            {
                hash_ ^= (~(hash_ << 11) ^ (str[i] ^ hash_ >> 5));
            }
        }
        return (hash_ & 0x7fffffff);
    }

    int32_t hash_4(const std::string& key)
    {
        int32_t     hash_ = 5381;
        std::string str   = key + "keykeykey4";
        auto        count = str.size();
        while (count > 0)
        {
            hash_ += (hash_ << 5) + str[str.size() - count];
            count--;
        }
        return (hash_ & 0x7fffffff);
    }
} // namespace Piccolo
