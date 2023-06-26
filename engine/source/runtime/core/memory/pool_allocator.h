#pragma once
#include "runtime/core/memory/allocator.h"

#include <iostream>
#include <list>
#include <memory>
#include <stack>

namespace Piccolo
{
    template<size_t BlockSize, size_t ReservedBlocks = 0>
    class Pool
    {
    private:
        size_t                                 m_size;
        std::stack<void*>                      m_addrs;
        std::stack<std::unique_ptr<uint8_t[]>> m_blocks;

    public:
        explicit Pool(size_t size) : m_size(size)
        {
            for (size_t i = 0; i < ReservedBlocks; i++)
            {
                add_more_addresses();
            }
        }

        void* allocate()
        {
            if (m_addrs.empty())
            {
                add_more_addresses();
            }

            auto ptr = m_addrs.top();
            m_addrs.pop();
            return ptr;
        }

        void deallocate(void* ptr) { m_addrs.push(ptr); }

        // Rebind should only be called by STL containers when they need to create
        // an allocator for an internal node-like structure from the value_type allocator.
        // This means that the original allocator must not have been used yet, so we
        // are free to reassign the size_ field safely.
        void rebind(size_t size)
        {
            if (!(m_addrs.empty() && m_blocks.empty()))
            {
                std::cerr << "Cannot call Pool::rebind() after an allocation\n";
                throw std::bad_alloc();
            }
            m_size = size;
        }

    private:
        // Refill the address stack by allocating another block of memory
        void add_more_addresses()
        {
            auto block      = std::make_unique<uint8_t[]>(BlockSize);
            auto total_size = BlockSize % m_size == 0 ? BlockSize : BlockSize - m_size;

            // Divide the allocated block into chunks of size_ bytes, and add their address
            for (size_t i = 0; i < total_size; i += m_size)
            {
                m_addrs.push(&block.get()[i]);
            }

            // Keep the memory of the block alive by adding it to our stack
            m_blocks.push(std::move(block));
        }
    };

    template<typename T, size_t BlockSize = 4096, size_t ReservedBlocks = 0>
    class PoolAllocator
    {
    private:
        using PoolType = Pool<BlockSize, ReservedBlocks>;
        std::shared_ptr<PoolType> pool_;

    public:
        using value_type      = T;
        using is_always_equal = std::false_type;

        PoolAllocator() : pool_(std::make_shared<PoolType>(sizeof(T))) {}

        // Rebind copy constructor
        template<typename U>
        PoolAllocator(const PoolAllocator<U>& other) : pool_ {other.pool_}
        {
            pool_->rebind(sizeof(T));
        }

        template<typename U>
        struct rebind
        {
            using other = PoolAllocator<U, BlockSize, ReservedBlocks>;
        };

        PoolAllocator(const PoolAllocator& other)            = default;
        PoolAllocator(PoolAllocator&& other)                 = default;
        PoolAllocator& operator=(const PoolAllocator& other) = default;
        PoolAllocator& operator=(PoolAllocator&& other)      = default;

        T* allocate(size_t n)
        {
            if (n > 1)
            {
                // For n > 1, resort to using malloc
                return static_cast<T*>(malloc(sizeof(T) * n));
            }

            return static_cast<T*>(pool_->allocate());
        }

        void deallocate(T* ptr, size_t n)
        {
            if (n > 1)
            {
                free(ptr);
                return;
            }

            pool_->deallocate(ptr);
        }
    };
} // namespace Piccolo