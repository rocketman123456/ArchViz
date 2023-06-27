#pragma once
#include "runtime/core/memory/allocator.h"

#include <stdlib.h>

#define BLOCK_SIZE (4096)
#define CACHE_L1_LINE_SIZE (64)

namespace ArchViz
{
    void buddy_init(void* space, int block_num); // allocate buddy

    void* buddy_alloc(int n); // allocate page (size of page is 2^n)

    void buddy_free(void* space, int n); // free page (starting address is space, size of page is 2^n)

    void buddy_print(); // print current state of buddy
} // namespace ArchViz

namespace ArchViz
{
    typedef struct kmem_cache_s kmem_cache_t;

    void slab_init(void* space, int block_num);

    kmem_cache_t* slab_cache_create(const char* name, size_t size, void (*ctor)(void*), void (*dtor)(void*)); // Allocate cache

    int slab_cache_shrink(kmem_cache_t* cachep); // Shrink cache

    void* slab_cache_alloc(kmem_cache_t* cachep); // Allocate one object from cache

    void slab_cache_free(kmem_cache_t* cachep, void* objp); // Deallocate one object from cache

    void* slab_malloc(size_t size); // Alloacate one small memory buffer

    void slab_free(const void* objp); // Deallocate one small memory buffer

    void slab_cache_destroy(kmem_cache_t* cachep); // Deallocate cache

    void slab_cache_info(kmem_cache_t* cachep); // Print cache info

    int slab_cache_error(kmem_cache_t* cachep); // Print error message
} // namespace ArchViz