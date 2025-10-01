#include "nexus/exec/policy.hpp"
#include "nexus/exec/queue.hpp"
#include "nexus/exec/task.hpp"

#include <any>
#include <gtest/gtest.h>

namespace {

using nexus::exec::Task;
using nexus::exec::TaskPolicy;
using nexus::exec::TaskQueue;

template <typename T> auto unwrap_task(Task<std::any> &task) -> T {
    task();

    auto future = task.get_future();
    auto result = future.get();

    return std::any_cast<T>(result);
}

TEST(TaskQueue, FIFO) {
    auto fifo = TaskQueue(TaskPolicy::FIFO);

    fifo.emplace([]() { return 0; });
    fifo.emplace([]() { return 1; });
    fifo.emplace([]() { return 2; });

    auto task1 = fifo.pop();
    auto task2 = fifo.pop();
    auto task3 = fifo.pop();

    EXPECT_EQ(unwrap_task<int>(task1), 0);
    EXPECT_EQ(unwrap_task<int>(task2), 1);
    EXPECT_EQ(unwrap_task<int>(task3), 2);
}

TEST(TaskQueue, LIFO) {
    auto lifo = TaskQueue(TaskPolicy::LIFO);

    lifo.emplace([]() { return 0; });
    lifo.emplace([]() { return 1; });
    lifo.emplace([]() { return 2; });

    auto task1 = lifo.pop();
    auto task2 = lifo.pop();
    auto task3 = lifo.pop();

    EXPECT_EQ(unwrap_task<int>(task1), 2);
    EXPECT_EQ(unwrap_task<int>(task2), 1);
    EXPECT_EQ(unwrap_task<int>(task3), 0);
}

} // namespace
