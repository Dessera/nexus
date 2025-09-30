#include "nexus/exec/policy.hpp"
#include "nexus/exec/queue.hpp"
#include "nexus/exec/task.hpp"

#include <cassert>

namespace {

using nexus::exec::Task;

template <typename T> auto unwrap_task(Task<T> &task) -> T {
    task();

    auto future = task.get_future();
    return future.get();
}

} // namespace

auto main() -> int {
    using nexus::exec::TaskPolicy;
    using nexus::exec::TaskQueue;

    auto fifo = TaskQueue<int>();

    fifo.emplace([]() { return 0; });
    fifo.emplace([]() { return 1; });
    fifo.emplace([]() { return 2; });

    auto task1 = fifo.pop();
    auto task2 = fifo.pop();
    auto task3 = fifo.pop();

    assert(unwrap_task(task1) == 0);
    assert(unwrap_task(task2) == 1);
    assert(unwrap_task(task3) == 2);

    auto lifo = TaskQueue<int, TaskPolicy::LIFO>();

    lifo.emplace([]() { return 0; });
    lifo.emplace([]() { return 1; });
    lifo.emplace([]() { return 2; });

    task1 = lifo.pop();
    task2 = lifo.pop();
    task3 = lifo.pop();

    assert(unwrap_task(task1) == 2);
    assert(unwrap_task(task2) == 1);
    assert(unwrap_task(task3) == 0);
}
