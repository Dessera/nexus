#include "nexus/exec/policy.hpp"
#include "nexus/exec/queue.hpp"
#include "nexus/exec/task.hpp"
#include "nexus/exec/worker.hpp"

#include <future>
#include <gtest/gtest.h>
#include <memory>

namespace {

using nexus::exec::Task;
using nexus::exec::TaskPolicy;
using nexus::exec::TaskQueue;
using nexus::exec::Worker;

template <typename T> auto unwrap_future(std::future<std::any> &fut) -> T {
    auto result = fut.get();
    return std::any_cast<T>(result);
}

TEST(Worker, SingleWorker) {
    auto queue = std::make_shared<TaskQueue>(TaskPolicy::FIFO);

    auto worker = Worker(queue);
    EXPECT_TRUE(worker.run());

    auto task = Task([]() { return 42; }); // NOLINT
    auto task_future = task.get_future();

    queue->push(std::move(task));

    EXPECT_NO_THROW(EXPECT_EQ(unwrap_future<int>(task_future), 42));

    EXPECT_TRUE(worker.cancel());
    queue->wakeup_all();
}

TEST(Worker, MultiWorker) {
    auto queue = std::make_shared<TaskQueue>(TaskPolicy::FIFO);

    auto worker1 = Worker(queue);
    EXPECT_TRUE(worker1.run());

    auto worker2 = Worker(queue);
    EXPECT_TRUE(worker2.run());

    auto task1 = Task([]() { return 1; });
    auto task1_future = task1.get_future();

    auto task2 = Task([]() { return 2; });
    auto task2_future = task2.get_future();

    auto task3 = Task([]() { return 3; });
    auto task3_future = task3.get_future();

    queue->push(std::move(task1));
    queue->push(std::move(task2));
    queue->push(std::move(task3));

    EXPECT_NO_THROW(EXPECT_EQ(unwrap_future<int>(task1_future), 1));
    EXPECT_NO_THROW(EXPECT_EQ(unwrap_future<int>(task2_future), 2));
    EXPECT_NO_THROW(EXPECT_EQ(unwrap_future<int>(task3_future), 3));

    EXPECT_TRUE(worker1.cancel());
    EXPECT_TRUE(worker2.cancel());
    queue->wakeup_all();
}

} // namespace
