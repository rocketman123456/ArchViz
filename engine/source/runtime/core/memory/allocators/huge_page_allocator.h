#pragma once
#include "runtime/core/memory/allocator.h"
#include "runtime/core/memory/memory_utils.h"

#include <cstdlib>
#include <limits>
#include <new>

namespace ArchViz
{
    template<typename T, size_t HugePageSize = 1 << 21>
    class THPAllocator
    {
    public:
        using is_always_equal = std::true_type;
        using value_type      = T;

        template<typename U>
        struct rebind
        {
            using other = THPAllocator<U, HugePageSize>;
        };

        THPAllocator() = default;

        template<class U>
        constexpr THPAllocator(const THPAllocator<U>& other)
        {
            (void)other;
        }

        T* allocate(size_t n)
        {
            if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
            {
                throw std::bad_alloc();
            }
            const auto total_size = n * sizeof(T);
            void*      p          = nullptr;
            if (posix_memalign(&p, HugePageSize, total_size) != 0)
            {
                throw std::bad_alloc();
            }

            madvise(p, total_size, MADV_HUGEPAGE);
            if (p == nullptr)
            {
                throw std::bad_alloc();
            }

            return static_cast<T*>(p);
        }

        void deallocate(T* p, size_t n)
        {
            (void)n;
            free(p);
        }
    };
} // namespace ArchViz
