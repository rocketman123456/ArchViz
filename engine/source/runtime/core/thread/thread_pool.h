// https://github.com/mvorbrodt/blog/blob/master/src/pool.hpp
#pragma once

#include "runtime/core/container/queue.h"

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace Piccolo
{
    class ThreadPool
    {
    public:
        explicit ThreadPool(std::size_t thread_count = std::thread::hardware_concurrency()) : m_queues(thread_count), m_count(thread_count)
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

        ~ThreadPool()
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

    extern ThreadPool g_thread_pool;
} // namespace Piccolo
