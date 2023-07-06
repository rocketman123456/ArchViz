#pragma once

#include <cstdint>

#if defined(_WIN32)
#include <malloc.h>
#include <stdio.h>
// #define posix_memalign(p, a, s) (((*(p)) = _aligned_malloc((s), (a))), *(p) ?0 :errno)
int check_align(size_t align);

int posix_memalign(void** ptr, size_t align, size_t size);
#else
#include <sys/mman.h> // for posix_memalign
#endif

namespace ArchViz
{
    // Memory Methods /////////////////////////////////////////////////////
    void memory_copy(void* destination, void* source, size_t size);

    //  Calculate aligned memory size.
    size_t memory_align(size_t size, size_t alignment);

    // Memory Structs /////////////////////////////////////////////////////
    //
    //
    struct MemoryStatistics
    {
        size_t allocated_bytes;
        size_t total_bytes;

        uint32_t allocation_count;

        void add(size_t a)
        {
            if (a)
            {
                allocated_bytes += a;
                ++allocation_count;
            }
        }
    }; // struct MemoryStatistics
} // namespace ArchViz
