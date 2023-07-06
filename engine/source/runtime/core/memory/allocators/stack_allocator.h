#pragma once
#include "runtime/core/memory/allocator.h"

#include <memory>

namespace ArchViz
{
    template<typename T, size_t BlockSize>
    class StackAllocator : public Allocator<T, BlockSize>
    {
        using Header = unsigned char;
        std::size_t m_offset;

    public:
        template<typename U>
        StackAllocator(const StackAllocator<U>& other)
        {
            (void)other;
        }

        // Non-copy
        StackAllocator(const StackAllocator&)            = delete;
        StackAllocator& operator=(const StackAllocator&) = delete;
        StackAllocator(StackAllocator&&)                 = delete;
        StackAllocator& operator=(StackAllocator&&)      = delete;

        StackAllocator() : m_offset(0) { this->m_start_address = ::operator new(this->m_size); }

        ~StackAllocator()
        {
            ::operator delete(this->m_start_address);
            this->m_start_address = nullptr;
        }

        T* allocate(const std::size_t size_, const std::size_t alignment) override
        {
            std::size_t size = sizeof(T) * size_;

            void*       currentAddress = reinterpret_cast<char*>(this->m_start_address) + m_offset;
            void*       nextAddress    = reinterpret_cast<void*>(reinterpret_cast<char*>(currentAddress) + sizeof(Header));
            std::size_t space          = this->m_size - m_offset - sizeof(Header);
            std::align(alignment, size, nextAddress, space);

            if ((std::size_t)nextAddress + size > (std::size_t)this->m_start_address + this->m_size)
                return nullptr;

            std::size_t padding = (std::size_t)nextAddress - (std::size_t)currentAddress;

            Header* header = reinterpret_cast<Header*>(reinterpret_cast<char*>(nextAddress) - sizeof(Header));
            *header        = (Header)padding;

            m_offset = (std::size_t)nextAddress - (std::size_t)this->m_start_address + size;

            return reinterpret_cast<T*>(nextAddress);
        }

        void deallocate(T* ptr, std::size_t n) override
        {
            const std::size_t currentAddress = (std::size_t)ptr;
            Header*           header         = reinterpret_cast<Header*>(currentAddress - sizeof(Header));

            m_offset = currentAddress - (std::size_t)this->m_start_address - *header;
        }

        void reset() override { m_offset = 0; }
    };

} // namespace ArchViz
