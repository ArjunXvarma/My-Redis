#include "utils/ThreadPool.hpp"

thread_local WorkStealingQueue* ThreadPool::local_queue = nullptr;
thread_local unsigned ThreadPool::my_index = 0;

ThreadPool::ThreadPool() : done(false), joiner(threads) {
    const unsigned thread_count = std::thread::hardware_concurrency();

    try {
        for (unsigned i = 0; i < thread_count; ++i) {
            queues.push_back(std::unique_ptr<WorkStealingQueue>(new WorkStealingQueue()));
            threads.push_back(std::thread(&ThreadPool::worker_thread, this, i));
        }
    }
    catch (...) {
        done = true;
        throw;
    }
}

ThreadPool::~ThreadPool() {
    done = true;
}

void ThreadPool::worker_thread(unsigned my_index_) {
    my_index = my_index_;
    local_queue = queues[my_index].get();
    while (!done) {
        run_pending_task();
    }
}

bool ThreadPool::pop_task_from_local_queue(task_type& task) {
    return local_queue && local_queue->try_pop(task);
}

bool ThreadPool::pop_task_from_pool_queue(task_type& task) {
    return global_queue.try_pop(task);
}

bool ThreadPool::pop_task_from_other_thread(task_type& task) {
    for (int i = 0; i < queues.size(); ++i) {
        unsigned const index = (my_index + i + 1) % queues.size();
        if (queues[index]->try_steal(task)) return true;
    }
    return false;
}

void ThreadPool::run_pending_task() {
    task_type task;
    if (pop_task_from_local_queue(task) || pop_task_from_pool_queue(task) || pop_task_from_other_thread(task))
        task();
    else
        std::this_thread::yield();
}
