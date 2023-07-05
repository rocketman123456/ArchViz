// https://github.com/SaschaWillems/Vulkan/blob/master/base/threadpool.hpp
#pragma once

#include "runtime/core/base/macro.h"
#include "runtime/core/container/queue.h"
#include "runtime/core/thread/work_executor.h"

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <queue>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

// make_unique is not available in C++11
// Taken from Herb Sutter's blog (https://herbsutter.com/gotw/_102/)
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

namespace ArchViz
{
    class Thread
    {
    private:
        bool                              m_destroying = false;
        std::thread                       m_worker;
        std::queue<std::function<void()>> m_job_queue;
        std::mutex                        m_queue_mutex;
        std::condition_variable           m_condition;

        // Loop through all remaining jobs
        void queueLoop()
        {
            while (true)
            {
                std::function<void()> job;
                {
                    std::unique_lock<std::mutex> lock(m_queue_mutex);
                    m_condition.wait(lock, [this] { return !m_job_queue.empty() || m_destroying; });
                    if (m_destroying)
                    {
                        break;
                    }
                    job = m_job_queue.front();
                }

                job();

                {
                    std::lock_guard<std::mutex> lock(m_queue_mutex);
                    m_job_queue.pop();
                    m_condition.notify_one();
                }
            }
        }

    public:
        Thread() { m_worker = std::thread(&Thread::queueLoop, this); }

        ~Thread()
        {
            if (m_worker.joinable())
            {
                wait();
                m_queue_mutex.lock();
                m_destroying = true;
                m_condition.notify_one();
                m_queue_mutex.unlock();
                m_worker.join();
            }
        }

        // Add a new job to the thread's queue
        void addJob(std::function<void()> function)
        {
            std::lock_guard<std::mutex> lock(m_queue_mutex);
            m_job_queue.push(std::move(function));
            m_condition.notify_one();
        }

        // Wait until all work items have been finished
        void wait()
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_condition.wait(lock, [this]() { return m_job_queue.empty(); });
        }
    };

    class ThreadPool
    {
    public:
        std::vector<std::unique_ptr<Thread>> m_threads;

        // Sets the number of threads to be allocated in this pool
        void setThreadCount(uint32_t count)
        {
            m_threads.clear();
            for (uint32_t i = 0; i < count; i++)
            {
                m_threads.push_back(make_unique<Thread>());
            }
        }

        // Wait until all threads have finished their work items
        void wait()
        {
            for (auto& thread : m_threads)
            {
                thread->wait();
            }
        }
    };
} // namespace ArchViz
