#pragma once

namespace ArchViz
{
    template<typename Base, typename T>
    inline bool instance_of (const T* ptr)
    {
        return dynamic_cast<const Base*>(ptr) != nullptr;
    }
} // namespace ArchViz