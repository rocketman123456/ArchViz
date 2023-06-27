#pragma once

namespace ArchViz
{
    template<typename Base, typename T>
    inline bool instanceof (const T* ptr)
    {
        return dynamic_cast<const Base*>(ptr) != nullptr;
    }
} // namespace Piccolo