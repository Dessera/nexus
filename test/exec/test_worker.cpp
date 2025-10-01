#include "nexus/config.hpp"
#include "nexus/exec/queue.hpp"
#include "nexus/exec/task.hpp"
#include "nexus/exec/worker.hpp"

#include <gtest/gtest.h>
#include <memory>

namespace {

using nexus::exec::Task;
using nexus::exec::TaskQueue;
using nexus::exec::Worker;

TEST(NEXUS_TESTS_DELAY(Worker), SingleWorker) {
    auto queue = std::make_shared<TaskQueue<int>>();

    auto worker = Worker(queue);
    EXPECT_TRUE(worker.run());

    auto task = Task<int>([]() { return 42; }); // NOLINT
    auto task_future = task.get_future();

    queue->push(std::move(task));

    EXPECT_NO_THROW(EXPECT_EQ(task_future.get(), 42));
}

TEST(NEXUS_TESTS_DELAY(Worker), MultiWorker) {
    auto queue = std::make_shared<TaskQueue<int>>();

    auto worker1 = Worker(queue);
    EXPECT_TRUE(worker1.run());

    auto worker2 = Worker(queue);
    EXPECT_TRUE(worker2.run());

    auto task1 = Task<int>([]() { return 1; });
    auto task1_future = task1.get_future();

    auto task2 = Task<int>([]() { return 2; });
    auto task2_future = task2.get_future();

    auto task3 = Task<int>([]() { return 3; });
    auto task3_future = task3.get_future();

    queue->push(std::move(task1));
    queue->push(std::move(task2));
    queue->push(std::move(task3));

    EXPECT_NO_THROW(EXPECT_EQ(task1_future.get(), 1));
    EXPECT_NO_THROW(EXPECT_EQ(task2_future.get(), 2));
    EXPECT_NO_THROW(EXPECT_EQ(task3_future.get(), 3));
}

} // namespace
