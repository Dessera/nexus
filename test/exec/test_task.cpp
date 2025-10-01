#include "nexus/exec/task.hpp"

#include <future>
#include <gtest/gtest.h>
#include <stdexcept>

namespace {

using nexus::exec::Task;

TEST(Task, Success) {
    auto success_task =
        Task<double>([](int arg1, double arg2) { return arg1 + arg2; }, 1, 2);
    success_task();

    auto success_task_future = success_task.get_future();
    auto success_task_result = success_task_future.get();
    EXPECT_EQ(success_task_result, 3);
}

TEST(Task, SuccessVoid) {
    auto success_task_void = Task<void>([]() {});
    success_task_void();

    auto success_task_void_future = success_task_void.get_future();
    EXPECT_NO_THROW(success_task_void_future.get());
}

TEST(Task, Failed) {
    auto failed_task =
        Task<void>([]() { throw std::runtime_error("exception"); });
    failed_task(); // Should not throw here.

    auto failed_task_future = failed_task.get_future();
    EXPECT_THROW(failed_task_future.get(), std::runtime_error);
}

} // namespace
