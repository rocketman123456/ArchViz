#pragma once

#include <deque>
#include <unordered_map>
#include <exception>
#include <stdexcept>

namespace Piccolo
{
    template<typename S, typename T>
    class lru_cache
    {
    public:
        explicit lru_cache(std::size_t c) : capacity(c) {}

        T get(const S& key)
        {
            auto found = resource_map.find(key);
            if (found != resource_map.end())
            {
                std::pair<S, T> p = *(found->second);
                history.erase(resource_map[key]);
                history.push_front(p);
                (found->second) = history.begin();
                return p.second;
            }
            else
            {
                throw std::runtime_error("cannot find key in cache");
            }
        }

        void put(const S& key, const T& value)
        {
            auto found = resource_map.find(key);
            if (found != resource_map.end())
            {
                history.erase(found->second);
                history.push_front({key, value});
                (found->second) = history.begin();
            }
            else
            {
                if (history.size() >= capacity)
                {
                    auto last = history.back();
                    history.pop_back();
                    resource_map.erase(last.first);
                }
                history.push_front({key, value});
            }
            resource_map.insert({key, history.begin()});
        }

    private:
        using ResourceMap = std::unordered_map<S, typename std::deque<std::pair<S, T>>::iterator>;

        std::size_t                 capacity = 0;
        std::deque<std::pair<S, T>> history;
        ResourceMap                 resource_map;
    };
} // namespace Piccolo
