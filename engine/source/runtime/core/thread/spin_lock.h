#pragma once

#include <atomic>

namespace ArchViz
{
    class SpinLock
    {
    private:
        std::atomic_flag m_atomic_flag = ATOMIC_FLAG_INIT;

    public:
        void lock()
        {
            for (;;)
            {
                if (!m_atomic_flag.test_and_set(std::memory_order_acquire))
                {
                    break;
                }
                while (m_atomic_flag.test(std::memory_order_relaxed))
                {}
            }
        }
        void unlock() { m_atomic_flag.clear(std::memory_order_release); }
    };
} // namespace ArchViz
