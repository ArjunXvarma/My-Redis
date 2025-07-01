#pragma once

#include "utils/ThreadPool.hpp"

template<typename FunctionType>
auto ThreadPool::enqueue(FunctionType f) -> std::future<std::invoke_result_t<FunctionType>> {
    using result_type = std::invoke_result_t<FunctionType>;

    std::packaged_task<result_type()> task(std::move(f));
    std::future<result_type> res = task.get_future();

    if (local_queue) local_queue->push(std::move(task));
    else global_queue.push(std::move(task));

    return res;
}