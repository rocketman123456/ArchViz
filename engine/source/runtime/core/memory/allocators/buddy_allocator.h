#pragma once
#include "runtime/core/memory/allocator.h"

#include <array>
#include <cmath>
#include <ctgmath>
#include <iostream>
#include <limits>
#include <memory>

namespace ArchViz
{
    constexpr std::size_t Log2(std::size_t x) { return x == 1 ? 0 : 1 + Log2(x / 2); }

    template<typename T, size_t BlockSize>
    class BuddyAllocator : public Allocator<T, BlockSize>
    {
        struct Node
        {
            Node* m_next;
        };

        struct alignas(std::max_align_t) Header
        {
            std::size_t m_size;
        };

        static constexpr std::size_t s_log2_header = Log2(sizeof(Header));

    public:
        std::array<Node*, std::numeric_limits<std::size_t>::digits - s_log2_header> m_buckets = {};

        // Non-copy
        BuddyAllocator(const BuddyAllocator&) = delete;
        BuddyAllocator& operator=(const BuddyAllocator&) = delete;
        BuddyAllocator(BuddyAllocator&&)                 = delete;
        BuddyAllocator& operator=(BuddyAllocator&&) = delete;

        BuddyAllocator()
        {
            this->m_start_address = ::operator new(this->m_size);
            init();
        }

        ~BuddyAllocator() override
        {
            ::operator delete(this->m_start_address);
            this->m_start_address = nullptr;
        }

        template<typename U, size_t BZ>
        BuddyAllocator(const BuddyAllocator<U, BZ>& other)
        {
            (void)other;
        }

        T* allocate(const std::size_t size_, const std::size_t alignment) override
        {
            std::size_t size   = sizeof(T) * size_;
            int         bucket = std::max(int(std::ceil(std::log2(size + sizeof(Header))) - 1 - s_log2_header), 0);

            if (m_buckets[bucket] != nullptr)
            {
                Node* node        = m_buckets[bucket];
                m_buckets[bucket] = node->m_next;
                Header* header    = reinterpret_cast<Header*>(node);
                header->m_size    = (std::size_t)std::pow(2, bucket + 1 + s_log2_header) | 1;
                void* address     = (void*)(reinterpret_cast<char*>(node) + sizeof(Header));
                return reinterpret_cast<T*>(address);
            }

            int i;

            for (i = bucket + 1; i < m_buckets.size(); ++i)
            {
                if (m_buckets[i] != nullptr)
                    break;
            }

            if (i == m_buckets.size())
                return nullptr;

            Node* temp   = m_buckets[i];
            m_buckets[i] = temp->m_next;

            --i;

            for (; i >= bucket; i--)
            {
                Node* node   = reinterpret_cast<Node*>(reinterpret_cast<char*>(temp) + (std::size_t)std::pow(2, i + 1 + s_log2_header));
                node->m_next = m_buckets[i];
                m_buckets[i] = node;
            }

            Header* header = reinterpret_cast<Header*>(temp);
            header->m_size = (std::size_t)std::pow(2, i + 2 + s_log2_header) | 1;
            void* address  = (void*)(reinterpret_cast<char*>(temp) + sizeof(Header));

            return reinterpret_cast<T*>(address);
        }

        void deallocate(T* ptr, std::size_t n) override
        {
            Header* header = reinterpret_cast<Header*>(reinterpret_cast<char*>(ptr) - sizeof(Header));

            const std::size_t size   = header->m_size & ~(std::size_t)1;
            const std::size_t bucket = (std::size_t)std::log2(size) - 1 - s_log2_header;

            Node* node = reinterpret_cast<Node*>(header);

            std::size_t buddyNumber;
            char*       buddyAddress;

            buddyNumber = (reinterpret_cast<char*>(header) - static_cast<char*>(this->m_start_address)) / size;

            if (buddyNumber % 2 == 0)
                buddyAddress = reinterpret_cast<char*>(header) + size;
            else
                buddyAddress = reinterpret_cast<char*>(header) - size;

            // Check if buddy is occupied to bale early from searching for it
            if (buddyAddress == (static_cast<char*>(this->m_start_address) + this->m_size) || *reinterpret_cast<std::size_t*>(buddyAddress) & 1)
            {
                node->m_next      = m_buckets[bucket];
                m_buckets[bucket] = node;
            }
            else
            {
                Node* prevBuddy = nullptr;
                Node* buddy     = reinterpret_cast<Node*>(buddyAddress);
                Node* context   = m_buckets[bucket];

                // Search the bucket for the buddy and update linked listed
                // This could be improved from O(N) to O(LogN) with RBTree of addresses of nodes
                while (context != buddy && context != nullptr)
                {
                    prevBuddy = context;
                    context   = context->m_next;
                }

                // If buddy was not found in the bucket it was probably split so we can't merge
                if (context == nullptr)
                {
                    node->m_next      = m_buckets[bucket];
                    m_buckets[bucket] = node;
                }
                else
                {
                    if (prevBuddy == nullptr)
                    {
                        m_buckets[bucket] = buddy->m_next;
                    }
                    else
                    {
                        prevBuddy->m_next = buddy->m_next;
                    }

                    if (buddyNumber % 2 == 0)
                    {
                        node->m_next          = m_buckets[bucket + 1];
                        m_buckets[bucket + 1] = node;
                    }
                    else
                    {
                        buddy->m_next         = m_buckets[bucket + 1];
                        m_buckets[bucket + 1] = buddy;
                    }
                }
            }
        }

        void reset() override
        {
            const std::size_t bucket = (std::size_t)std::ceil(std::log2(this->m_size)) - 1 - s_log2_header;
            for (std::size_t i = 0; i < bucket; ++i)
            {
                m_buckets[i] = nullptr;
            }
            init();
        }

    private:
        void init()
        {
            Node* root               = reinterpret_cast<Node*>(this->m_start_address);
            root->m_next             = nullptr;
            const std::size_t bucket = (std::size_t)std::ceil(std::log2(this->m_size)) - 1 - s_log2_header;
            m_buckets[bucket]        = root;
        }
    };

    // template<typename T, typename U>
    // bool operator==(const BuddyAllocator<T>& a1, const BuddyAllocator<U>& a2)
    // {
    //     (void)a1;
    //     (void)a2;
    //     return true;
    // }

    // template<typename T, typename U>
    // bool operator!=(const BuddyAllocator<T>& a1, const BuddyAllocator<U>& a2)
    // {
    //     (void)a1;
    //     (void)a2;
    //     return false;
    // }
} // namespace ArchViz
