#pragma once
#include "runtime/core/memory/allocator.h"

#include <cstdio>
#include <cstdlib>

namespace ArchViz
{
    template<typename T>
    class CacheAlignedAllocator : public Allocator<T, 0>
    {
    public:
        using value_type      = T;
        using is_always_equal = std::true_type;

        template<typename U>
        struct rebind
        {
            using other = CacheAlignedAllocator<U>;
        };

        CacheAlignedAllocator() = default;

        template<typename U>
        CacheAlignedAllocator(const CacheAlignedAllocator<U>& other)
        {
            (void)other;
        }

        T* allocate(size_t n, const std::size_t alignment) override
        {
#ifdef _WIN32
            auto ptr = static_cast<T*>(_aligned_malloc(alignment, sizeof(T) * n));
#else
            auto ptr = static_cast<T*>(std::aligned_alloc(alignment, sizeof(T) * n));
#endif
            if (ptr)
                return ptr;

            throw std::bad_alloc();
        }

        void deallocate(T* ptr, size_t n) override
        {
            (void)n;
#ifdef _WIN32
            _aligned_free(ptr);
#else
            std::free(ptr);
#endif
        }

        void reset() override {}
    };
} // namespace ArchViz