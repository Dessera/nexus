#include "nexus/config.hpp"
#include "nexus/exec/pool.hpp"

#include <cstddef>
#include <gtest/gtest.h>

namespace {

using nexus::exec::ThreadPool;

constexpr std::size_t MaxWorkers = 5;
constexpr std::size_t MinWorkers = 1;

TEST(NEXUS_TESTS_DELAY(Pool), Simple) {
    auto pool = ThreadPool<int>(MaxWorkers, MinWorkers);

    auto task1_future = pool.emplace([]() { return 1; });
    auto task2_future = pool.emplace([]() { return 2; });
    auto task3_future = pool.emplace([]() { return 3; });

    EXPECT_NO_THROW(EXPECT_EQ(task1_future.get(), 1));
    EXPECT_NO_THROW(EXPECT_EQ(task2_future.get(), 2));
    EXPECT_NO_THROW(EXPECT_EQ(task3_future.get(), 3));
}

TEST(NEXUS_TESTS_DELAY(Pool), ResizePool) {
    auto pool = ThreadPool<int>(MaxWorkers, MinWorkers);

    auto task1_future = pool.emplace([]() { return 1; });
    auto task2_future = pool.emplace([]() { return 2; });
    auto task3_future = pool.emplace([]() { return 3; });

    pool.resize_workers(4);

    auto task4_future = pool.emplace([]() { return 4; });
    auto task5_future = pool.emplace([]() { return 5; }); // NOLINT
    auto task6_future = pool.emplace([]() { return 6; }); // NOLINT

    pool.resize_workers(1);

    EXPECT_NO_THROW(EXPECT_EQ(task1_future.get(), 1));
    EXPECT_NO_THROW(EXPECT_EQ(task2_future.get(), 2));
    EXPECT_NO_THROW(EXPECT_EQ(task3_future.get(), 3));
    EXPECT_NO_THROW(EXPECT_EQ(task4_future.get(), 4));
    EXPECT_NO_THROW(EXPECT_EQ(task5_future.get(), 5));
    EXPECT_NO_THROW(EXPECT_EQ(task6_future.get(), 6));
}

} // namespace
