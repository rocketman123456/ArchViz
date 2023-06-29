// https://github.com/mvorbrodt/blog/blob/master/src/pool.hpp
// https://github.com/SaschaWillems/Vulkan/blob/master/base/threadpool.hpp
#pragma once

#include "runtime/core/container/queue.h"

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
    class WorkExecutor
    {
    public:
        explicit WorkExecutor(std::size_t thread_count = std::thread::hardware_concurrency()) : m_queues(thread_count), m_count(thread_count)
        {
            if (!thread_count)
                throw std::invalid_argument("bad thread count! must be non-zero!");

            auto worker = [this](auto i) {
                while (true)
                {
                    proc_t f;
                    for (auto n = 0; n < m_count * K; ++n)
                        if (m_queues[(i + n) % m_count].try_pop(f))
                            break;
                    if (!f && !m_queues[i].pop(f))
                        break;
                    f();
                }
            };

            m_threads.reserve(thread_count);
            for (auto i = 0; i < thread_count; ++i)
            {
                m_threads.emplace_back(worker, i);
            }
        }

        ~WorkExecutor()
        {
            for (auto& queue : m_queues)
                queue.unblock();
            for (auto& thread : m_threads)
                thread.join();
        }

        template<typename F, typename... Args>
        void enqueue_work(F&& f, Args&&... args)
        {
            auto work = [p = std::forward<F>(f), t = std::make_tuple(std::forward<Args>(args)...)]() { std::apply(p, t); };
            auto i    = m_index++;

            for (auto n = 0; n < m_count * K; ++n)
                if (m_queues[(i + n) % m_count].try_push(work))
                    return;

            m_queues[i % m_count].push(std::move(work));
        }

        template<typename F, typename... Args>
        [[nodiscard]] auto enqueue_task(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
        {
            using task_return_type = std::invoke_result_t<F, Args...>;
            using task_type        = std::packaged_task<task_return_type()>;

            auto task   = std::make_shared<task_type>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
            auto work   = [=]() { (*task)(); };
            auto result = task->get_future();
            auto i      = m_index++;

            for (auto n = 0; n < m_count * K; ++n)
                if (m_queues[(i + n) % m_count].try_push(work))
                    return result;

            m_queues[i % m_count].push(std::move(work));
            return result;
        }

    private:
        using proc_t   = std::function<void(void)>;
        using queue_t  = unbounded_queue<proc_t>;
        using queues_t = std::vector<queue_t>;
        queues_t m_queues;

        using threads_t = std::vector<std::thread>;
        threads_t m_threads;

        const std::size_t m_count;
        std::atomic_uint  m_index = 0;

        inline static const unsigned int K = 2;
    };

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
            for (auto i = 0; i < count; i++)
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
