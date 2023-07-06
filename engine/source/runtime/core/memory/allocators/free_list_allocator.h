#pragma once
#include "runtime/core/container/linked_list.h"
#include "runtime/core/memory/allocator.h"

#include <cassert>
#include <memory>

namespace ArchViz
{
    template<typename T, size_t BlockSize>
    class FreeListAllocator : public Allocator<T, BlockSize>
    {
    public:
        enum class SearchMethod
        {
            FIRST,
            BEST
        };

    private:
        struct alignas(std::max_align_t) Header
        {
            std::size_t m_size;
        };
        linked_list  m_list;
        SearchMethod m_search_method;

    public:
        template<typename U, size_t BZ>
        FreeListAllocator(const FreeListAllocator<U, BZ>& other)
        {
            (void)other;
        }

        // Non-copy
        FreeListAllocator(const FreeListAllocator&)            = delete;
        FreeListAllocator& operator=(const FreeListAllocator&) = delete;
        FreeListAllocator(FreeListAllocator&&)                 = delete;
        FreeListAllocator& operator=(FreeListAllocator&&)      = delete;

        FreeListAllocator(const SearchMethod searchMethod = BEST)
        {
            static std::string message =
                "Total size must be atleast " + std::to_string(sizeof(linked_list::Node) + 1) + " bytes for an allocator with atleast 1 byte of free space";
            assert(this->m_size >= sizeof(linked_list::Node) + 1 && message.c_str());
            m_search_method       = searchMethod;
            this->m_start_address = ::operator new(this->m_size);
            init();
        }

        ~FreeListAllocator() override
        {
            ::operator delete(this->m_start_address);
            this->m_start_address = nullptr;
        }

        T* allocate(const std::size_t size_, const std::size_t alignment)
        {
            std::size_t size = sizeof(T) * size_;
            std::size_t padding;
            void*       currentAddress = (void*)(sizeof(Header) + size);
            void*       nextAddress    = (void*)(sizeof(Header) + size);
            std::size_t space          = size + 100;
            std::align(alignof(std::max_align_t), sizeof(std::max_align_t), nextAddress, space);
            padding = (std::size_t)nextAddress - (std::size_t)currentAddress;

            linked_list::Node* prev;
            linked_list::Node* best;

            switch (m_search_method)
            {
                case SearchMethod::FIRST:
                    m_list.search_first(size + padding, best, prev);
                    break;
                case SearchMethod::BEST:
                    m_list.search_best(size + padding, best, prev);
                    break;
            }

            if (best == nullptr)
            {
                return nullptr;
            }

            if (best->m_value >= size + padding + sizeof(linked_list::Node*) + 1)
            {
                linked_list::Node* splittedNode = reinterpret_cast<linked_list::Node*>(reinterpret_cast<char*>(best) + sizeof(Header) + size + padding);
                splittedNode->m_value           = best->m_value - (size + padding + sizeof(Header));
                splittedNode->m_next            = best->m_next;
                best->m_next                    = splittedNode;
            }
            else
            {
                padding += best->m_value - (size + padding);
            }

            if (prev == nullptr)
            {
                m_list.m_head = best->m_next;
            }
            else
            {
                prev->m_next = best->m_next;
            }

            Header* header = reinterpret_cast<Header*>(best);
            header->m_size = size + padding;

            return reinterpret_cast<T*>(best) + sizeof(Header);
        }

        void deallocate(T* ptr, std::size_t n)
        {
            Header* header = reinterpret_cast<Header*>(reinterpret_cast<char*>(ptr) - sizeof(Header));

            linked_list::Node* node = reinterpret_cast<linked_list::Node*>(header);
            node->m_value           = header->m_size;

            linked_list::Node* prevIt = nullptr;
            linked_list::Node* it     = m_list.m_head;
            while (it != nullptr)
            {
                if (node < it)
                {
                    node->m_next = it;
                    if (prevIt == nullptr)
                    {
                        m_list.m_head = node;
                    }
                    else
                    {
                        prevIt->m_next = node;
                    }
                    break;
                }
                prevIt = it;
                it     = it->m_next;
            }

            coalescence(prevIt, node);
        }

        void reset() { init(); }

    private:
        void init()
        {
            linked_list::Node* head = reinterpret_cast<linked_list::Node*>(this->m_start_address);
            head->m_value           = this->m_size - sizeof(Header);
            head->m_next            = nullptr;
            m_list.m_head           = head;
        }

        void coalescence(linked_list::Node* prev, linked_list::Node* curr)
        {
            if (curr->m_next != nullptr && (std::size_t)curr + curr->m_value + sizeof(Header) == (std::size_t)curr->m_next)
            {
                curr->m_value += curr->m_next->m_value + sizeof(Header);
                curr->m_next = curr->m_next->m_next;
            }

            if (prev != nullptr && (std::size_t)prev + prev->m_value + sizeof(Header) == (std::size_t)curr)
            {
                prev->m_value += curr->m_value + sizeof(Header);
                prev->m_next = curr->m_next;
            }
        }
    };

} // namespace ArchViz
