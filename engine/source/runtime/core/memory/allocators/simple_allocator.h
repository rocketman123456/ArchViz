#pragma once
#include "runtime/core/memory/allocator.h"

#include <cstdlib>
#include <deque>

namespace ArchViz
{
    template<typename T>
    class SimpleAllocator : public Allocator<T>
    {
    public:
        using value_type = T;

        SimpleAllocator() = default;

        template<typename U>
        SimpleAllocator(const SimpleAllocator<U>& other)
        {
            (void)other;
        }

        T* allocate(const std::size_t size, const std::size_t alignment) override
        {
            auto ptr = static_cast<T*>(malloc(sizeof(T) * n));
            if (ptr)
                return ptr;

            throw std::bad_alloc();
        }

        void deallocate(T* ptr, std::size_t n) override
        {
            (void)n;
            free(ptr);
        }

        void reset() override {}
    };

    template<typename T, typename U>
    bool operator==(const SimpleAllocator<T>& a1, const SimpleAllocator<U>& a2)
    {
        (void)a1;
        (void)a2;
        return true;
    }

    template<typename T, typename U>
    bool operator!=(const SimpleAllocator<T>& a1, const SimpleAllocator<U>& a2)
    {
        (void)a1;
        (void)a2;
        return false;
    }
} // namespace ArchViz