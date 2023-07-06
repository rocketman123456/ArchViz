#include "runtime/core/memory/memory_utils.h"

#include "runtime/core/base/macro.h"

#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <malloc.h>
#include <stdio.h>
// #define posix_memalign(p, a, s) (((*(p)) = _aligned_malloc((s), (a))), *(p) ?0 :errno)
int check_align(size_t align)
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
#endif

namespace ArchViz
{
    // Memory Methods /////////////////////////////////////////////////////////
    void memory_copy(void* destination, void* source, size_t size)
    {
        ASSERT(destination);
        ASSERT(source);
        memcpy(destination, source, size);
    }

    size_t memory_align(size_t size, size_t alignment)
    {
        const size_t alignment_mask = alignment - 1;
        return (size + alignment_mask) & ~alignment_mask;
    }
} // namespace ArchViz