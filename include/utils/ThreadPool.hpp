#pragma once

#include <vector>
#include <thread>
#include <future>
#include <atomic>
#include <type_traits>
#include "Queue.hpp"
#include "common.hpp"
#include "WorkStealingQueue.hpp"

class ThreadPool {
    using task_type = function_wrapper;

    std::atomic_bool done;
    Queue<task_type> global_queue;
    std::vector<std::unique_ptr<WorkStealingQueue>> queues;
    std::vector<std::thread> threads;
    join_threads joiner;

    static thread_local WorkStealingQueue* local_queue;
    static thread_local unsigned my_index;

    void worker_thread(unsigned my_index_);
    bool pop_task_from_local_queue(task_type& task);
    bool pop_task_from_pool_queue(task_type& task);
    bool pop_task_from_other_thread(task_type& task);

public:
    ThreadPool();
    ~ThreadPool();

    template<typename FunctionType>
    auto enqueue(FunctionType f) -> std::future<std::invoke_result_t<FunctionType>>;

    void run_pending_task();
};

#include "../src/utils/ThreadPool.tpp"