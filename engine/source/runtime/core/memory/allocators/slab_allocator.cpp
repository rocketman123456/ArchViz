#include "runtime/core/memory/allocators/slab_allocator.h"

#include <cmath>
#include <cstring>
#include <iostream>
#include <mutex>

constexpr int BLOCK_SIZE         = 4096;
constexpr int CACHE_L1_LINE_SIZE = 64;

using namespace std;

namespace ArchViz
{
    void* buddySpace;
    int   numOfEntries;
    int   startingBlockNum;

    void** freeList;

    void buddy_init(void* space, int block_num)
    {
        if (space == nullptr || block_num < 2)
            exit(1); // broj blokova mora biti veci od 1 (1 blok odlazi na buddy)

        startingBlockNum = block_num;
        buddySpace       = space;
        space            = ((char*)space + BLOCK_SIZE); // ostatak memorije se koristi za alokaciju
        block_num--;                                    // prvi blok ide za potrebe buddy alokatora

        int i        = 1;
        numOfEntries = log2f(block_num) + 1;

        freeList = (void**)buddySpace;
        for (i = 0; i < numOfEntries; i++)
            freeList[i] = nullptr;

        int maxLength       = numOfEntries - 1;
        int maxLengthBlocks = 1 << maxLength;

        while (block_num > 0)
        {
            void* addr          = (char*)space + (block_num - maxLengthBlocks) * BLOCK_SIZE;
            freeList[maxLength] = addr;
            *(void**)addr       = nullptr;
            block_num -= maxLengthBlocks;

            if (block_num > 0)
            {
                i         = 1;
                maxLength = 0;
                while (true)
                {
                    if (i <= block_num && 2 * i > block_num)
                        break;
                    i = i * 2;
                    maxLength++;
                }
                maxLengthBlocks = 1 << maxLength;
            }
        }
    }

    void* buddy_alloc(int n)
    {
        if (n < 0 || n >= numOfEntries)
            return nullptr;

        void* returningSpace = nullptr;

        if (freeList[n] != nullptr)
        {
            returningSpace          = freeList[n];
            freeList[n]             = *(void**)returningSpace;
            *(void**)returningSpace = nullptr;
        }
        else
        {
            for (int i = n + 1; i < numOfEntries; i++)
            {
                if (freeList[i] != nullptr)
                {
                    void* ptr1  = freeList[i];
                    freeList[i] = *(void**)ptr1;
                    void* ptr2  = (char*)ptr1 + BLOCK_SIZE * (1 << (i - 1));

                    *(void**)ptr1   = ptr2;
                    *(void**)ptr2   = freeList[i - 1];
                    freeList[i - 1] = ptr1;

                    returningSpace = buddy_alloc(n);
                    break;
                }
            }
        }

        return returningSpace;
    }

    inline bool isValid(void* space, int n) // check if starting address (space1) is valid for length 2^n
    {
        int length = 1 << n;
        int num    = ((startingBlockNum - 1) % length) + 1;
        int i      = ((char*)space - (char*)buddySpace) / BLOCK_SIZE; // num of first block

        if (i % length == num % length) // if starting block number is valid for length 2^n then true
            return true;

        return false;
    }

    void buddy_free(void* space, int n)
    {
        if (n < 0 || n >= numOfEntries)
            return;

        int bNum = 1 << n;

        if (freeList[n] == nullptr)
        {
            freeList[n]    = space;
            *(void**)space = nullptr;
        }
        else
        {
            void* prev = nullptr;
            void* curr = freeList[n];
            while (curr != nullptr)
            {
                if (curr == (void*)((char*)space + BLOCK_SIZE * bNum)) // right buddy potentially found
                {
                    if (isValid(space, n + 1)) // right buddy found
                    {
                        if (prev == nullptr)
                        {
                            freeList[n] = *(void**)freeList[n];
                        }
                        else
                        {
                            *(void**)prev = *(void**)curr;
                        }

                        buddy_free(space, n + 1);

                        return;
                    }
                }
                else if (space == (void*)((char*)curr + BLOCK_SIZE * bNum)) // left buddy potentially found
                {
                    if (isValid(curr, n + 1)) // left buddy found
                    {
                        if (prev == nullptr)
                        {
                            freeList[n] = *(void**)freeList[n];
                        }
                        else
                        {
                            *(void**)prev = *(void**)curr;
                        }

                        buddy_free(curr, n + 1);

                        return;
                    }
                }

                prev = curr;
                curr = *(void**)curr;
            }

            *(void**)space = freeList[n];
            freeList[n]    = space;
        }
    }

    void buddy_print()
    {
        cout << "Buddy current state (first block,last block):" << endl;
        for (int i = 0; i < numOfEntries; i++)
        {
            int size = 1 << i;
            cout << "entry[" << i << "] (size " << size << ") -> ";
            void* curr = freeList[i];

            while (curr != nullptr)
            {
                int first = ((char*)curr - (char*)buddySpace) / BLOCK_SIZE;
                cout << "(" << first << "," << first + size - 1 << ") -> ";
                curr = *(void**)curr;
            }
            cout << "NULL" << endl;
        }
    }
} // namespace ArchViz

#define CACHE_NAMELEN (20) // maximum length of cache name
#define CACHE_CACHE_ORDER (0) // cache_cache order

namespace ArchViz
{
    typedef struct slab_s
    {
        unsigned int  colouroff;   // offset for this slab
        void*         objects;     // starting adress of objects
        int*          freeList;    // list of free objects
        int           nextFreeObj; // next free object
        unsigned int  inuse;       // number of active objects in this slab
        slab_s*       next;        // next slab in chain
        slab_s*       prev;        // previous slab in chain
        kmem_cache_t* myCache;     // cache - owner
    } slab_t;

    struct kmem_cache_s
    {
        slab_t*       slabs_full;          // list of full slabs
        slab_t*       slabs_partial;       // list of partial slabs
        slab_t*       slabs_free;          // list of free slabs
        char          name[CACHE_NAMELEN]; // cache name
        unsigned int  objectSize;          // size of one object
        unsigned int  objectsInSlab;       // num of objects in one slab
        unsigned long num_active;          // num of active objects in cache
        unsigned long num_allocations;     // num of total objects in cache
        mutex         cache_mutex;         // mutex (uses to lock the cache)
        unsigned int  order;               // order of one slab (one slab has 2^order blocks)
        unsigned int  colour_max;          // maximum multiplier for offset of first object in slab
        unsigned int  colour_next;         // multiplier for next slab offset
        bool          growing;             // false - cache is not growing / true - cache is growing
        void (*ctor)(void*);               // objects constructor
        void (*dtor)(void*);               // objects destructor
        int           error_code;          // last error that happened while working with cache
        kmem_cache_s* next;                // next cache in chain
    };

    /*
    ERROR CODES: (error_cod value)
    0 - no error
    1 - invalid arguments in function slab_cache_create
    2 - no enough space for allocating new slab
    3 - users don't have access to cache_cache
    4 - nullpointer argument passed to func slab_cache_error
    5 - cache passed by func slab_cache_destroy does not exists in cache_cache
    6 - object passed by func slab_cache_free does not exists in cache_cache
    7 - invalid pointer passed for object dealocation
    */

    mutex buddy_mutex; // guarding buddy alocator
    mutex cout_mutex;  // guarding cout

    static kmem_cache_t cache_cache;

    static kmem_cache_t* allCaches = nullptr;

    void kmem_cache_allInfo()
    {
        kmem_cache_t* curr = allCaches;
        while (curr != nullptr)
        {
            slab_cache_info(curr);
            cout << endl;
            curr = curr->next;
        }
    }

    void slab_init(void* space, int block_num)
    {
        buddy_init(space, block_num);

        void* ptr = buddy_alloc(CACHE_CACHE_ORDER);
        if (ptr == nullptr)
            exit(1);
        slab_t* slab = (slab_t*)ptr;

        cache_cache.slabs_free    = slab;
        cache_cache.slabs_full    = nullptr;
        cache_cache.slabs_partial = nullptr;

        strcpy(cache_cache.name, "kmem_cache");
        cache_cache.objectSize = sizeof(kmem_cache_t);
        cache_cache.order      = CACHE_CACHE_ORDER;

        cache_cache.growing    = false;
        cache_cache.ctor       = nullptr;
        cache_cache.dtor       = nullptr;
        cache_cache.error_code = 0;
        cache_cache.next       = nullptr;

        slab->colouroff   = 0;
        slab->freeList    = (int*)((char*)ptr + sizeof(slab_t));
        slab->nextFreeObj = 0;
        slab->inuse       = 0;
        slab->next        = nullptr;
        slab->prev        = nullptr;
        slab->myCache     = &cache_cache;

        long memory = (1 << cache_cache.order) * BLOCK_SIZE;
        memory -= sizeof(slab_t);
        int n = 0;
        while ((long)(memory - sizeof(unsigned int) - cache_cache.objectSize) >= 0)
        {
            n++;
            memory -= sizeof(unsigned int) + cache_cache.objectSize;
        }

        slab->objects      = (void*)((char*)ptr + sizeof(slab_t) + sizeof(unsigned int) * n);
        kmem_cache_t* list = (kmem_cache_t*)slab->objects;

        for (int i = 0; i < n; i++)
        {
            *list[i].name = '\0';
            // memcpy(&list[i].cache_mutex, &mutex(), sizeof(mutex));
            new (&list[i].cache_mutex) mutex;
            slab->freeList[i] = i + 1;
        }
        // slab->freeList[n - 1] = -1;

        cache_cache.objectsInSlab   = n;
        cache_cache.num_active      = 0;
        cache_cache.num_allocations = n;

        cache_cache.colour_max = memory / CACHE_L1_LINE_SIZE;
        if (cache_cache.colour_max > 0)
            cache_cache.colour_next = 1;
        else
            cache_cache.colour_next = 0;

        allCaches = &cache_cache;
    }

    kmem_cache_t* slab_cache_create(const char* name, size_t size, void (*ctor)(void*), void (*dtor)(void*)) // Allocate cache
    {
        if (name == nullptr || *name == '\0' || (long)size <= 0)
        {
            cache_cache.error_code = 1;
            return nullptr;
        }

        if (strcmp(name, cache_cache.name) == 0)
        {
            cache_cache.error_code = 3;
            return nullptr;
        }

        lock_guard<mutex> guard(cache_cache.cache_mutex);

        kmem_cache_t* ret      = nullptr;
        cache_cache.error_code = 0; // reset error code

        slab_t* s;

        // prvi nacin
        ret = allCaches;
        while (ret != nullptr)
        {
            if (strcmp(ret->name, name) == 0 && ret->objectSize == size)
                return ret;
            ret = ret->next;
        }

        // drugi nacin
        /*
        s = cache_cache.slabs_full;
        while (s != nullptr)	// check if cache already exists in slabs_full list
        {
            kmem_cache_t* list = (kmem_cache_t*)s->objects;
            for (int i = 0; i < cache_cache.objectsInSlab; i++)
            {
                if (strcmp(list[i].name, name) == 0 && list[i].objectSize == size)
                    return &list[i];
            }
            s = s->next;
        }
        s = cache_cache.slabs_partial;
        while (s != nullptr)	// check if cache already exists in slabs_partial list
        {
            kmem_cache_t* list = (kmem_cache_t*)s->objects;
            for (int i = 0; i < cache_cache.objectsInSlab; i++)
            {
                if (strcmp(list[i].name, name) == 0 && list[i].objectSize == size)
                    return &list[i];
            }
            s = s->next;
        }
        */

        // cache does not exists

        s = cache_cache.slabs_partial;
        if (s == nullptr)
            s = cache_cache.slabs_free;

        if (s == nullptr) // there is not enough space, try to allocate more space for cache_cache
        {
            lock_guard<mutex> guard(buddy_mutex);
            void*             ptr = buddy_alloc(CACHE_CACHE_ORDER);
            if (ptr == nullptr)
            {
                cache_cache.error_code = 2;
                return nullptr;
            }
            s = (slab_t*)ptr;

            cache_cache.slabs_partial = s;

            s->colouroff            = cache_cache.colour_next;
            cache_cache.colour_next = (cache_cache.colour_next + 1) % (cache_cache.colour_max + 1);

            s->freeList    = (int*)((char*)ptr + sizeof(slab_t));
            s->nextFreeObj = 0;
            s->inuse       = 0;
            s->next        = nullptr;
            s->prev        = nullptr;
            s->myCache     = &cache_cache;

            s->objects         = (void*)((char*)ptr + sizeof(slab_t) + sizeof(unsigned int) * cache_cache.objectsInSlab + CACHE_L1_LINE_SIZE * s->colouroff);
            kmem_cache_t* list = (kmem_cache_t*)s->objects;

            for (int i = 0; i < cache_cache.objectsInSlab; i++)
            {
                *list[i].name = '\0';
                // memcpy(&list[i].cache_mutex, &mutex(), sizeof(mutex));
                new (&list[i].cache_mutex) mutex;
                s->freeList[i] = i + 1;
            }
            // s->freeList[cache_cache.objectsInSlab - 1] = -1;

            cache_cache.num_allocations += cache_cache.objectsInSlab;

            cache_cache.growing = true;
        }

        kmem_cache_t* list = (kmem_cache_t*)s->objects;
        ret                = &list[s->nextFreeObj];
        s->nextFreeObj     = s->freeList[s->nextFreeObj];
        s->inuse++;
        cache_cache.num_active++;

        if (s == cache_cache.slabs_free)
        {
            cache_cache.slabs_free = s->next;
            if (cache_cache.slabs_free != nullptr)
                cache_cache.slabs_free->prev = nullptr;

            if (s->inuse != cache_cache.objectsInSlab) // from free to partial
            {
                s->next = cache_cache.slabs_partial;
                if (cache_cache.slabs_partial != nullptr)
                    cache_cache.slabs_partial->prev = s;
                cache_cache.slabs_partial = s;
            }
            else // from free to full
            {
                s->next = cache_cache.slabs_full;
                if (cache_cache.slabs_full != nullptr)
                    cache_cache.slabs_full->prev = s;
                cache_cache.slabs_full = s;
            }
        }
        else
        {
            if (s->inuse == cache_cache.objectsInSlab) // from partial to full
            {
                cache_cache.slabs_partial = s->next;
                if (cache_cache.slabs_partial != nullptr)
                    cache_cache.slabs_partial->prev = nullptr;

                s->next = cache_cache.slabs_full;
                if (cache_cache.slabs_full != nullptr)
                    cache_cache.slabs_full->prev = s;
                cache_cache.slabs_full = s;
            }
        }

        // initialise new cache
        strcpy(ret->name, name);

        ret->slabs_full    = nullptr;
        ret->slabs_partial = nullptr;
        ret->slabs_free    = nullptr;

        ret->growing    = false;
        ret->ctor       = ctor;
        ret->dtor       = dtor;
        ret->error_code = 0;
        ret->next       = allCaches;
        allCaches       = ret;

        // finding new cache order

        long memory = BLOCK_SIZE;
        int  order  = 0;
        while ((long)(memory - sizeof(slab_t) - sizeof(unsigned int) - size) < 0)
        {
            order++;
            memory *= 2;
        }

        ret->objectSize = size;
        ret->order      = order;

        // finding number of objects in slab

        memory -= sizeof(slab_t);
        int n = 0;
        while ((long)(memory - sizeof(unsigned int) - size) >= 0)
        {
            n++;
            memory -= sizeof(unsigned int) + size;
        }

        ret->objectsInSlab   = n;
        ret->num_active      = 0;
        ret->num_allocations = 0;

        ret->colour_max  = memory / CACHE_L1_LINE_SIZE;
        ret->colour_next = 0;

        return ret;
    }

    int slab_cache_shrink(kmem_cache_t* cachep) // Shrink cache
    {
        if (cachep == nullptr)
            return 0;

        lock_guard<mutex> guard(cachep->cache_mutex);

        int blocksFreed    = 0;
        cachep->error_code = 0;
        if (cachep->slabs_free != nullptr && cachep->growing == false) // if there is any slab in slab_free list and growing==false
        {
            lock_guard<mutex> guard(buddy_mutex);
            int               n = 1 << cachep->order;
            slab_t*           s;
            while (cachep->slabs_free != nullptr)
            {
                s                  = cachep->slabs_free;
                cachep->slabs_free = s->next;
                buddy_free(s, cachep->order);
                blocksFreed += n;
                cachep->num_allocations -= cachep->objectsInSlab;
            }
        }
        cachep->growing = false; // reset growing flag
        return blocksFreed;
    }

    void* slab_cache_alloc(kmem_cache_t* cachep) // Allocate one object from cache
    {
        if (cachep == nullptr || *cachep->name == '\0')
            return nullptr;

        lock_guard<mutex> guard(cachep->cache_mutex);

        void* retObject    = nullptr;
        cachep->error_code = 0;

        slab_t* s = cachep->slabs_partial;
        if (s == nullptr)
            s = cachep->slabs_free;
        if (s == nullptr) // alloc new slab
        {
            lock_guard<mutex> guard(buddy_mutex);

            void* ptr = buddy_alloc(cachep->order);
            if (ptr == nullptr)
            {
                cachep->error_code = 2;
                return nullptr;
            }
            s = (slab_t*)ptr;

            cachep->slabs_partial = s; // object will be allocated from this slab => slab will be in partial used list

            s->colouroff        = cachep->colour_next;
            cachep->colour_next = (cachep->colour_next + 1) % (cachep->colour_max + 1);

            s->freeList    = (int*)((char*)ptr + sizeof(slab_t));
            s->nextFreeObj = 0;
            s->inuse       = 0;
            s->next        = nullptr;
            s->prev        = nullptr;
            s->myCache     = cachep;

            s->objects = (void*)((char*)ptr + sizeof(slab_t) + sizeof(unsigned int) * cachep->objectsInSlab + CACHE_L1_LINE_SIZE * s->colouroff);
            void* obj  = s->objects;

            for (int i = 0; i < cachep->objectsInSlab; i++)
            {
                if (cachep->ctor)
                    cachep->ctor(obj);
                obj            = (void*)((char*)obj + cachep->objectSize);
                s->freeList[i] = i + 1;
            }
            // s->freeList[cachep->objectsInSlab - 1] = -1;

            cachep->num_allocations += cachep->objectsInSlab;

            cachep->growing = true;
        }

        // slab found

        retObject      = (void*)((char*)s->objects + s->nextFreeObj * cachep->objectSize);
        s->nextFreeObj = s->freeList[s->nextFreeObj];
        s->inuse++;
        cachep->num_active++;

        if (s == cachep->slabs_free)
        {
            cachep->slabs_free = s->next;
            if (cachep->slabs_free != nullptr)
                cachep->slabs_free->prev = nullptr;

            if (s->inuse != cachep->objectsInSlab) // from free to partial
            {
                s->next = cachep->slabs_partial;
                if (cachep->slabs_partial != nullptr)
                    cachep->slabs_partial->prev = s;
                cachep->slabs_partial = s;
            }
            else // from free to full
            {
                s->next = cachep->slabs_full;
                if (cachep->slabs_full != nullptr)
                    cachep->slabs_full->prev = s;
                cachep->slabs_full = s;
            }
        }
        else
        {
            if (s->inuse == cachep->objectsInSlab) // from partial to full
            {
                cachep->slabs_partial = s->next;
                if (cachep->slabs_partial != nullptr)
                    cachep->slabs_partial->prev = nullptr;

                s->next = cachep->slabs_full;
                if (cachep->slabs_full != nullptr)
                    cachep->slabs_full->prev = s;
                cachep->slabs_full = s;
            }
        }

        return retObject;
    }

    void slab_cache_free(kmem_cache_t* cachep, void* objp) // Deallocate one object from cache
    {
        if (cachep == nullptr || *cachep->name == '\0' || objp == nullptr)
            return;

        lock_guard<mutex> guard(cachep->cache_mutex);

        cachep->error_code = 0;
        slab_t* s;

        // find owner slab

        int  slabSize   = BLOCK_SIZE * (1 << cachep->order);
        bool inFullList = true; // slab s is in slabs_full list
        s               = cachep->slabs_full;
        while (s != nullptr)
        {
            if ((void*)objp > (void*)s && (void*)objp < (void*)((char*)s + slabSize))
                break;
            s = s->next;
        }

        if (s == nullptr)
        {
            inFullList = false; // slab s is in slabs_partial list
            s          = cachep->slabs_partial;
            while (s != nullptr)
            {
                if ((void*)objp > (void*)s && (void*)objp < (void*)((char*)s + slabSize))
                    break;
                s = s->next;
            }
        }

        if (s == nullptr)
        {
            cachep->error_code = 6;
            return;
        }

        // slab found, return object to slab

        s->inuse--;
        cachep->num_active--;
        int i = ((char*)objp - (char*)s->objects) / cachep->objectSize;
        if (objp != (void*)((char*)s->objects + i * cachep->objectSize))
        {
            cachep->error_code = 7;
            return;
        }
        s->freeList[i] = s->nextFreeObj;
        s->nextFreeObj = i;

        if (cachep->dtor != nullptr)
            cachep->dtor(objp);

        // if (cachep->ctor != nullptr)		destructor is responsible for returning object to initialised state
        //	cachep->ctor(objp);

        // check if slab is now free or partial used

        if (inFullList) // s is in full list
        {
            slab_t *prev, *next;

            // delete slab from full list
            prev    = s->prev;
            next    = s->next;
            s->prev = nullptr;

            if (prev != nullptr)
                prev->next = next;
            if (next != nullptr)
                next->prev = prev;
            if (cachep->slabs_full == s)
                cachep->slabs_full = next;

            if (s->inuse != 0) // insert slab to partial used list
            {
                s->next = cachep->slabs_partial;
                if (cachep->slabs_partial != nullptr)
                    cachep->slabs_partial->prev = s;
                cachep->slabs_partial = s;
            }
            else // insert slab to free list
            {
                s->next = cachep->slabs_free;
                if (cachep->slabs_free != nullptr)
                    cachep->slabs_free->prev = s;
                cachep->slabs_free = s;
            }
        }
        else // s is in partial list
        {
            if (s->inuse == 0)
            {
                slab_t *prev, *next;

                // delete slab from partial list

                prev    = s->prev;
                next    = s->next;
                s->prev = nullptr;

                if (prev != nullptr)
                    prev->next = next;
                if (next != nullptr)
                    next->prev = prev;
                if (cachep->slabs_partial == s)
                    cachep->slabs_partial = next;

                // insert slab to free list

                s->next = cachep->slabs_free;
                if (cachep->slabs_free != nullptr)
                    cachep->slabs_free->prev = s;
                cachep->slabs_free = s;
            }
        }
    }

    void* slab_malloc(size_t size) // Alloacate one small memory buffer
    {
        if (size < 32 || size > 131072)
            return nullptr;

        // int j = 1 << (int)(ceil(log2(size)));
        int j = 32;
        while (j < size)
            j <<= 1;

        char  num[7];
        void* buff = nullptr;

        char name[20];
        strcpy(name, "size-");
        sprintf(num, "%d", j);
        strcat(name, num);

        kmem_cache_t* buffCachep = slab_cache_create(name, j, nullptr, nullptr);

        buff = slab_cache_alloc(buffCachep);

        return buff;
    }

    kmem_cache_t* find_buffers_cache(const void* objp)
    {

        lock_guard<mutex> guard(cache_cache.cache_mutex);

        kmem_cache_t* curr = allCaches;
        slab_t*       s;

        while (curr != nullptr)
        {
            if (strstr(curr->name, "size-") != nullptr) // found small buffer-s cache
            {
                s            = curr->slabs_full;
                int slabSize = BLOCK_SIZE * (1 << curr->order);
                while (s != nullptr) // check if cache exists in slabs_full list or
                {
                    if ((void*)objp > (void*)s && (void*)objp < (void*)((char*)s + slabSize)) // cache found
                        return curr;

                    s = s->next;
                }

                s = curr->slabs_partial;
                while (s != nullptr) // check if cache exists in slabs_partial list
                {
                    if ((void*)objp > (void*)s && (void*)objp < (void*)((char*)s + slabSize)) // cache found
                        return curr;

                    s = s->next;
                }
            }
            curr = curr->next;
        }

        return nullptr;
    }

    void slab_free(const void* objp) // Deallocate one small memory buffer
    {
        if (objp == nullptr)
            return;

        kmem_cache_t* buffCachep = find_buffers_cache(objp);

        if (buffCachep == nullptr)
            return;

        slab_cache_free(buffCachep, (void*)objp);

        if (buffCachep->slabs_free != nullptr) // shrink buffer-s cache (save memory if usage is low)
            slab_cache_shrink(buffCachep);
    }

    void slab_cache_destroy(kmem_cache_t* cachep) // Deallocate cache
    {
        if (cachep == nullptr || *cachep->name == '\0')
            return;

        lock_guard<mutex> guard1(cachep->cache_mutex);
        lock_guard<mutex> guard2(cache_cache.cache_mutex);
        lock_guard<mutex> guard3(buddy_mutex);

        slab_t* s;
        void*   ptr;
        cache_cache.error_code = 0;

        // delete cache from allCaches list

        kmem_cache_t *prev = nullptr, *curr = allCaches;
        while (curr != cachep)
        {
            prev = curr;
            curr = curr->next;
        }

        if (curr == nullptr) // cache is not in cache chain (that means that object is not in cache_cache too)
        {
            cache_cache.error_code = 5;
            return;
        }

        if (prev == nullptr)
            allCaches = allCaches->next;
        else
            prev->next = curr->next;
        curr->next = nullptr;

        // find owner slab in cache_cache

        int  slabSize   = BLOCK_SIZE * (1 << cache_cache.order);
        bool inFullList = true; // slab s is in slabs_full list
        s               = cache_cache.slabs_full;
        while (s != nullptr)
        {
            if ((void*)cachep > (void*)s && (void*)cachep < (void*)((char*)s + slabSize))
                break;
            s = s->next;
        }

        if (s == nullptr)
        {
            inFullList = false; // slab s is in slabs_partial list
            s          = cache_cache.slabs_partial;
            while (s != nullptr)
            {
                if ((void*)cachep > (void*)s && (void*)cachep < (void*)((char*)s + slabSize))
                    break;
                s = s->next;
            }
        }

        if (s == nullptr) // owner slab not found in cache_cache
        {
            cache_cache.error_code = 5;
            return;
        }

        // owner slab found

        // reset cache fields and update cache_cache fields

        s->inuse--;
        cache_cache.num_active--;
        int i              = cachep - (kmem_cache_t*)s->objects;
        s->freeList[i]     = s->nextFreeObj;
        s->nextFreeObj     = i;
        *cachep->name      = '\0';
        cachep->objectSize = 0;

        // free used slabs

        slab_t* freeTemp = cachep->slabs_full;
        while (freeTemp != nullptr)
        {
            ptr      = freeTemp;
            freeTemp = freeTemp->next;
            buddy_free(ptr, cachep->order);
        }

        freeTemp = cachep->slabs_partial;
        while (freeTemp != nullptr)
        {
            ptr      = freeTemp;
            freeTemp = freeTemp->next;
            buddy_free(ptr, cachep->order);
        }

        freeTemp = cachep->slabs_free;
        while (freeTemp != nullptr)
        {
            ptr      = freeTemp;
            freeTemp = freeTemp->next;
            buddy_free(ptr, cachep->order);
        }

        // check if slab is now free or partial used (s is slab in cache_cache)
        if (inFullList) // s is in full list
        {
            slab_t *prev, *next;

            // delete slab from full list

            prev    = s->prev;
            next    = s->next;
            s->prev = nullptr;

            if (prev != nullptr)
                prev->next = next;
            if (next != nullptr)
                next->prev = prev;
            if (cache_cache.slabs_full == s)
                cache_cache.slabs_full = next;

            if (s->inuse != 0) // insert slab to partial used list
            {
                s->next = cache_cache.slabs_partial;
                if (cache_cache.slabs_partial != nullptr)
                    cache_cache.slabs_partial->prev = s;
                cache_cache.slabs_partial = s;
            }
            else // insert slab to free list
            {
                s->next = cache_cache.slabs_free;
                if (cache_cache.slabs_free != nullptr)
                    cache_cache.slabs_free->prev = s;
                cache_cache.slabs_free = s;
            }
        }
        else // s is in partial list
        {
            if (s->inuse == 0)
            {
                slab_t *prev, *next;

                // delete slab from partial list

                prev    = s->prev;
                next    = s->next;
                s->prev = nullptr;

                if (prev != nullptr)
                    prev->next = next;
                if (next != nullptr)
                    next->prev = prev;
                if (cache_cache.slabs_partial == s)
                    cache_cache.slabs_partial = next;

                // insert slab to free list

                s->next = cache_cache.slabs_free;
                if (cache_cache.slabs_free != nullptr)
                    cache_cache.slabs_free->prev = s;
                cache_cache.slabs_free = s;
            }
        }

        // if there is more than one slab in slab_free, free them

        if (cache_cache.slabs_free != nullptr)
        {
            s = cache_cache.slabs_free;
            i = 0;
            while (s != nullptr)
            {
                i++;
                s = s->next;
            }

            while (i > 1)
            {
                i--;
                s                            = cache_cache.slabs_free;
                cache_cache.slabs_free       = cache_cache.slabs_free->next;
                s->next                      = nullptr;
                cache_cache.slabs_free->prev = nullptr;
                buddy_free(s, cache_cache.order);
                cache_cache.num_allocations -= cache_cache.objectsInSlab;
            }
        }
    }

    void slab_cache_info(kmem_cache_t* cachep) // Print cache info
    {
        lock_guard<mutex> guard1(cout_mutex);

        if (cachep == nullptr)
        {
            cout << "NullPointer passed as argument" << endl;
            return;
        }

        lock_guard<mutex> guard2(cachep->cache_mutex);

        int i = 0;

        slab_t* s = cachep->slabs_free;
        while (s != nullptr)
        {
            i++;
            s = s->next;
        }

        s = cachep->slabs_partial;
        while (s != nullptr)
        {
            i++;
            s = s->next;
        }

        s = cachep->slabs_full;
        while (s != nullptr)
        {
            i++;
            s = s->next;
        }

        unsigned int cacheSize = i * (1 << cachep->order);

        double perc = 0;
        if (cachep->num_allocations > 0)
            perc = 100 * (double)cachep->num_active / cachep->num_allocations;

        cout << "*** CACHE INFO: ***" << endl
             << "Name:\t\t\t\t" << cachep->name << endl
             << "Size of one object (in bytes):\t" << cachep->objectSize << endl
             << "Size of cache (in blocks):\t" << cacheSize << endl
             << "Number of slabs:\t\t" << i << endl
             << "Number of objects in one slab:\t" << cachep->objectsInSlab << endl
             << "Percentage occupancy of cache:\t" << perc << " %" << endl;
    }

    int slab_cache_error(kmem_cache_t* cachep) // Print error message
    {
        lock_guard<mutex> guard1(cout_mutex);

        if (cachep == nullptr)
        {
            cout << "Nullpointer argument passed" << endl;
            return 4;
        }

        lock_guard<mutex> guard2(cachep->cache_mutex);

        int error_code = cachep->error_code;

        if (error_code == 0)
        {
            cout << "NO ERROR" << endl;
            return 0;
        }

        cout << "ERROR: ";
        switch (error_code)
        {
            case 1:
                cout << "Invalid arguments passed in function slab_cache_create" << endl;
                break;
            case 2:
                cout << "No enough space for allocating new slab" << endl;
                break;
            case 3:
                cout << "Access to cache_cache isn't allowed" << endl;
                break;
            case 4:
                cout << "NullPointer argument passed to func slab_cache_error" << endl;
                break;
            case 5:
                cout << "Cache passed by func slab_cache_destroy does not exists in kmem_cache" << endl;
                break;
            case 6:
                cout << "Object passed by func slab_cache_free does not exists in kmem_cache" << endl;
                break;
            case 7:
                cout << "Invalid pointer passed for object dealocation" << endl;
                break;
            default:
                cout << "Undefined error" << endl;
                break;
        }

        return error_code;
    }

    /*
    ERROR CODES: (error_cod value)
    0 - no error
    1 - invalid arguments in function slab_cache_create
    2 - no enough space for allocating new slab
    3 - users don't have access to cache_cache
    4 - nullpointer argument passed to func slab_cache_error
    5 - cache passed by func slab_cache_destroy does not exists in cache_cache
    6 - object passed by func slab_cache_free does not exists in cache_cache
    7 - invalid pointer passed for object dealocation
    */
} // namespace ArchViz
