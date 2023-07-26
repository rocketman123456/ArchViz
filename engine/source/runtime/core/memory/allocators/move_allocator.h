#pragma once
#include "runtime/core/memory/allocator.h"

#include <cstdlib>

namespace ArchViz
{
    template<class T>
    class MoveAllocator
    {
        T*     buffer;
        size_t offset;
        size_t reserved;

        static const size_t preserved = 100;

    public:
        using value_type = T;

        using pointer         = T*;
        using const_pointer   = const T*;
        using reference       = T&;
        using const_reference = const T&;

        MoveAllocator(MoveAllocator<T>&& oth) : buffer {oth.buffer}, offset {oth.offset}, reserved {oth.reserved}
        {
            std::cout << "move allocator";
            oth.buffer = nullptr;
        }

        MoveAllocator(const MoveAllocator<T>&) = delete;

        MoveAllocator() : reserved {preserved}, offset {0}
        {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
            buffer = new T[preserved];
        }

        ~MoveAllocator()
        {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
            ;
            delete[] buffer;
        }

        T* allocate(const std::size_t size, const std::size_t alignment)
        {
            if (offset + size > reserved)
            {
                throw std::bad_alloc();
                // std::cout << "offset+size" << offset+size << std::endl;
            }

            offset += size;
            auto old = buffer;
            buffer += size;
            return old;
        }

        template<typename U, typename... Args>
        void construct(U* p, Args&&... args)
        {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
            new (p) U(std::forward<Args>(args)...);
        };

        void deallocate(T*, std::size_t size) { std::cout << __PRETTY_FUNCTION__ << std::endl; }

        void destroy(T* p) { std::cout << __PRETTY_FUNCTION__ << std::endl; }

    private:
        void new_buffer()
        {
            pointer old = buffer;
            reserved <<= 1;
            buffer = new T[reserved];
            std::memcpy(buffer, old, sizeof(T));
        }
    };
} // namespace ArchViz