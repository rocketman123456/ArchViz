#pragma once

#include <atomic>
#include <cstdint>
#include <string>

#ifndef ENKI_ASSERT
#include <assert.h>
#define ENKI_ASSERT(x) assert(x)
#endif

namespace ArchViz
{
    // LockLessMultiReadPipe - Single writer, multiple reader thread safe pipe using (semi) lockless programming
    // Readers can only read from the back of the pipe
    // The single writer can write to the front of the pipe, and read from both ends (a writer can be a reader)
    // for many of the principles used here, see http://msdn.microsoft.com/en-us/library/windows/desktop/ee418650(v=vs.85).aspx
    // Note: using log2 sizes so we do not need to clamp (multi-operation)
    // T is the contained type
    // Note this is not true lockless as the use of flags as a form of lock state.
    template<uint8_t SIZE_LOG2, typename T>
    class LockLessMultiReadPipe
    {
    public:
        LockLessMultiReadPipe();
        ~LockLessMultiReadPipe() = default;

        // readerTryReadBack returns false if we were unable to read
        // This is thread safe for both multiple readers and the writer
        bool readerTryReadBack(T* out);

        // writerTryReadFront returns false if we were unable to read
        // This is thread safe for the single writer, but should not be called by readers
        bool writerTryReadFront(T* out);

        // writerTryWriteFront returns false if we were unable to write
        // This is thread safe for the single writer, but should not be called by readers
        bool writerTryWriteFront(const T& in);

        // isPipeEmpty() is a utility function, not intended for general use
        // Should only be used very prudently.
        bool isPipeEmpty() const { return 0 == m_write_index.load(std::memory_order_relaxed) - m_read_count.load(std::memory_order_relaxed); }

        void clear()
        {
            m_write_index = 0;
            m_read_index  = 0;
            m_read_count  = 0;
            memset((void*)m_flags, 0, sizeof(m_flags));
        }

    private:
        const static uint32_t ms_size        = (1 << SIZE_LOG2);
        const static uint32_t ms_index_mask  = ms_size - 1;
        const static uint32_t FLAG_INVALID   = 0xFFFFFFFF; // 32bit for CAS
        const static uint32_t FLAG_CAN_WRITE = 0x00000000; // 32bit for CAS
        const static uint32_t FLAG_CAN_READ  = 0x11111111; // 32bit for CAS

        T m_buffer[ms_size];

        // read and write indexes allow fast access to the pipe, but actual access
        // controlled by the access flags.
        std::atomic<uint32_t> m_write_index;
        std::atomic<uint32_t> m_read_count;
        std::atomic<uint32_t> m_flags[ms_size];
        std::atomic<uint32_t> m_read_index;
    };

    template<uint8_t SIZE_LOG2, typename T>
    inline LockLessMultiReadPipe<SIZE_LOG2, T>::LockLessMultiReadPipe() : m_write_index(0), m_read_count(0), m_read_index(0)
    {
        ENKI_ASSERT(SIZE_LOG2 < 32);
        memset((void*)m_flags, 0, sizeof(m_flags));
    }

    template<uint8_t SIZE_LOG2, typename T>
    inline bool LockLessMultiReadPipe<SIZE_LOG2, T>::readerTryReadBack(T* out)
    {
        uint32_t actual_read_index;
        uint32_t read_count = m_read_count.load(std::memory_order_relaxed);

        // We get hold of read index for consistency
        // and do first pass starting at read count
        uint32_t read_index_to_use = read_count;
        while (true)
        {

            uint32_t write_index = m_write_index.load(std::memory_order_relaxed);
            // power of two sizes ensures we can use a simple calc without modulus
            uint32_t num_in_pipe = write_index - read_count;
            if (0 == num_in_pipe)
            {
                return false;
            }
            if (read_index_to_use >= write_index)
            {
                read_index_to_use = m_read_index.load(std::memory_order_relaxed);
            }

            // power of two sizes ensures we can perform AND for a modulus
            actual_read_index = read_index_to_use & ms_index_mask;

            // Multiple potential readers mean we should check if the data is valid,
            // using an atomic compare exchange
            uint32_t previous = FLAG_CAN_READ;
            bool     success  = m_flags[actual_read_index].compare_exchange_strong(previous, FLAG_INVALID, std::memory_order_acq_rel, std::memory_order_relaxed);
            if (success)
            {
                break;
            }
            ++read_index_to_use;

            // Update read count
            read_count = m_read_count.load(std::memory_order_relaxed);
        }

        // we update the read index using an atomic add, as we've only read one piece of data.
        // this ensure consistency of the read index, and the above loop ensures readers
        // only read from unread data
        m_read_count.fetch_add(1, std::memory_order_relaxed);

        // now read data, ensuring we do so after above reads & CAS
        *out = m_buffer[actual_read_index];

        m_flags[actual_read_index].store(FLAG_CAN_WRITE, std::memory_order_release);

        return true;
    }

    template<uint8_t SIZE_LOG2, typename T>
    inline bool LockLessMultiReadPipe<SIZE_LOG2, T>::writerTryReadFront(T* out)
    {
        uint32_t write_index      = m_write_index.load(std::memory_order_relaxed);
        uint32_t front_read_index = write_index;

        // Multiple potential readers mean we should check if the data is valid,
        // using an atomic compare exchange - which acts as a form of lock (so not quite lockless really).
        uint32_t actual_read_index = 0;
        while (true)
        {
            uint32_t read_count = m_read_count.load(std::memory_order_relaxed);
            // power of two sizes ensures we can use a simple calc without modulus
            uint32_t num_in_pipe = write_index - read_count;
            if (0 == num_in_pipe)
            {
                m_read_index.store(read_count, std::memory_order_release);
                return false;
            }
            --front_read_index;
            actual_read_index = front_read_index & ms_index_mask;
            uint32_t previous = FLAG_CAN_READ;
            bool     success  = m_flags[actual_read_index].compare_exchange_strong(previous, FLAG_INVALID, std::memory_order_acq_rel, std::memory_order_relaxed);
            if (success)
            {
                break;
            }
            else if (m_read_index.load(std::memory_order_acquire) >= front_read_index)
            {
                return false;
            }
        }

        // now read data, ensuring we do so after above reads & CAS
        *out = m_buffer[actual_read_index];

        m_flags[actual_read_index].store(FLAG_CAN_WRITE, std::memory_order_relaxed);

        m_write_index.store(write_index - 1, std::memory_order_relaxed);
        return true;
    }

    template<uint8_t SIZE_LOG2, typename T>
    inline bool LockLessMultiReadPipe<SIZE_LOG2, T>::writerTryWriteFront(const T& in)
    {
        // The writer 'owns' the write index, and readers can only reduce
        // the amount of data in the pipe.
        // We get hold of both values for consistency and to reduce false sharing
        // impacting more than one access
        uint32_t write_index = m_write_index;

        // power of two sizes ensures we can perform AND for a modulus
        uint32_t actual_write_index = write_index & ms_index_mask;

        // a reader may still be reading this item, as there are multiple readers
        if (m_flags[actual_write_index].load(std::memory_order_acquire) != FLAG_CAN_WRITE)
        {
            return false; // still being read, so have caught up with tail.
        }

        // as we are the only writer we can update the data without atomics
        //  whilst the write index has not been updated
        m_buffer[actual_write_index] = in;
        m_flags[actual_write_index].store(FLAG_CAN_READ, std::memory_order_release);

        m_write_index.fetch_add(1, std::memory_order_relaxed);
        return true;
    }

    // Lockless multiwriter intrusive list
    // Type T must implement T* volatile next;
    template<typename T>
    class LocklessMultiWriteIntrusiveList
    {
        std::atomic<T*> head;
        T               tail;

    public:
        LocklessMultiWriteIntrusiveList() : head(&tail) { tail.next = NULL; }

        bool isListEmpty() const { return head == &tail; }

        // Add - safe to perform from any thread
        void writerWriteFront(T* pNode_)
        {
            ENKI_ASSERT(pNode_);
            pNode_->next = NULL;
            T* prev      = head.exchange(pNode_);
            prev->next   = pNode_;
        }

        // Remove - only thread safe for owner
        T* readerReadBack()
        {
            T* tail_plus_1 = tail.next;
            if (tail_plus_1)
            {
                T* tail_plus_2 = tail_plus_1->next;
                if (tail_plus_2)
                {
                    // not head
                    tail.next = tail_plus_2;
                }
                else
                {
                    tail.next  = NULL;
                    T* compare = tail_plus_1; // we need preserve tail_plus_1 as compare will alter it on failure
                    // tail_plus_1 is the head, attempt swap with tail
                    if (!head.compare_exchange_strong(compare, &tail))
                    {
                        // compare receives the revised head on failure.
                        // tail_plus_1 is no longer the head, so tail_plus_1->next should be non NULL
                        while ((T*)NULL == tail_plus_1->next)
                        {} // wait for next to be updated as head may have just changed.
                        tail.next         = tail_plus_1->next.load();
                        tail_plus_1->next = NULL;
                    }
                }
            }
            return tail_plus_1;
        }
    };
} // namespace ArchViz
