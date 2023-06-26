#pragma once

#include <cstddef>
#include <memory>

namespace Piccolo
{
    template<class T, size_t BlockSize>
    class Allocator;

    template<size_t BlockSize>
    class Allocator<void, BlockSize>
    {
    public:
        using value_type    = void;
        using pointer       = void*;
        using const_pointer = const void*;
        template<class U, size_t BZ>
        struct rebind
        {
            using other = Allocator<U, BZ>;
        };
    };

    // TODO : use template parameter to set total allocator size
    template<typename T, size_t BlockSize>
    class Allocator
    {
    public:
        using value_type      = T;
        using pointer         = value_type*;
        using const_pointer   = const value_type*;
        using reference       = value_type&;
        using const_reference = const value_type&;
        using size_type       = std::size_t;
        using difference_type = std::ptrdiff_t;
        using is_always_equal = std::true_type;

        std::size_t m_size {BlockSize};
        void*       m_start_address;

        Allocator()                            = default;
        Allocator(const Allocator&)            = default;
        Allocator& operator=(const Allocator&) = default;
        virtual ~Allocator()                   = default;

        template<class U, size_t BZ>
        Allocator(const Allocator<U, BZ>&) noexcept {};

        template<class U, size_t BZ>
        Allocator& operator=(const Allocator<U, BZ>&) noexcept
        {
            return *this;
        };

        virtual T*   allocate(const std::size_t size_) { return allocate(size_, 0); }
        virtual T*   allocate(const std::size_t size, const std::size_t alignment) = 0;
        virtual void deallocate(T* ptr, std::size_t n)                             = 0;
        virtual void reset()                                                       = 0;

        using propagate_on_container_move_assignment = std::true_type;
        template<class U, size_t BZ>
        struct rebind
        {
            using other = Allocator<U, BZ>;
        };

        template<class U, class... Args>
        void construct(U* ptr, Args&&... args)
        {
            ::new ((void*)ptr) U(std::forward<Args>(args)...);
        }

        template<class U>
        void destroy(U* ptr)
        {
            ptr->~U();
        }

        pointer       address(reference x) const noexcept { return std::addressof(x); }
        const_pointer address(const_reference x) const noexcept { return std::addressof(x); }
    };

    template<class T1, size_t BlockSize1, class T2, size_t BlockSize2>
    constexpr bool operator==(const Allocator<T1, BlockSize1>& lhs, const Allocator<T2, BlockSize2>& rhs)
    {
        return true;
    }

    template<class T1, size_t BlockSize1, class T2, size_t BlockSize2>
    constexpr bool operator!=(const Allocator<T1, BlockSize1>& lhs, const Allocator<T2, BlockSize2>& rhs)
    {
        return false;
    }
} // namespace Piccolo