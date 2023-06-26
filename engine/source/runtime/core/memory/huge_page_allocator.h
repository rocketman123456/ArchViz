#pragma once
#include "runtime/core/memory/allocator.h"

#include <cstdlib>
#include <limits>
#include <new>

#if defined(_WIN32)
#include <malloc.h>
#include <stdio.h>
//#define posix_memalign(p, a, s) (((*(p)) = _aligned_malloc((s), (a))), *(p) ?0 :errno)
static int check_align(size_t align)
{
    for (size_t i = sizeof(void*); i != 0; i *= 2)
        if (align == i)
            return 0;
    return EINVAL;
}

int posix_memalign(void** ptr, size_t align, size_t size)
{
    if (check_align(align))
        return EINVAL;

    int   saved_errno = errno;
    void* p           = _aligned_malloc(size, align);
    if (p == NULL)
    {
        errno = saved_errno;
        return ENOMEM;
    }

    *ptr = p;
    return 0;
}
#else
#include <sys/mman.h> // for posix_memalign

namespace Piccolo
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
} // namespace Piccolo
#endif