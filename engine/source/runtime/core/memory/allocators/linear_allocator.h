#pragma once
#include "runtime/core/memory/allocator.h"

#include <memory>

namespace ArchViz
{
    template<typename T, size_t BlockSize>
    class LinearAllocator : public Allocator<T, BlockSize>
    {
    private:
        std::size_t m_offset;

    public:
        template<typename U>
        LinearAllocator(const LinearAllocator<U>& other)
        {
            (void)other;
        }

        // Non-copy
        LinearAllocator(const LinearAllocator&)            = delete;
        LinearAllocator& operator=(const LinearAllocator&) = delete;
        LinearAllocator(LinearAllocator&&)                 = delete;
        LinearAllocator& operator=(LinearAllocator&&)      = delete;

        LinearAllocator() : m_offset(0) { m_start_address = ::operator new(size); }

        ~LinearAllocator()
        {
            ::operator delete(this->m_start_address);
            this->m_start_address = nullptr;
        }

        T* allocate(const std::size_t size_, const std::size_t alignment) override
        {
            std::size_t size           = sizeof(T) * size_;
            void*       currentAddress = reinterpret_cast<char*>(this->m_start_address) + m_offset;
            std::size_t space          = this->m_size - m_offset;
            std::align(alignment, size, currentAddress, space);

            if ((std::size_t)currentAddress + size > (std::size_t)this->m_start_address + this->m_size)
                return nullptr;

            m_offset = this->m_size - space + size;

            return reinterpret_cast<T*>(currentAddress);
        }

        void deallocate(T* ptr, std::size_t n) override {}

        void reset() override { m_offset = 0; }
    };

} // namespace ArchViz
