#pragma once
#include <iostream>
#include <mutex>

namespace ArchViz
{
    namespace
    {
        static inline std::mutex std_out_lock;
    }

    template<typename... Ts>
    inline void trace(Ts&&... args)
    {
        std::scoped_lock lock(std_out_lock);
        (std::cout << ... << args) << std::endl;
    }
} // namespace Piccolo