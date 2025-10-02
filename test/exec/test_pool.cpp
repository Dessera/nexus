#include "nexus/exec/builder.hpp"
#include "nexus/exec/pool.hpp"

#include <gtest/gtest.h>

namespace {

using nexus::exec::default_builder;

template <typename T> auto unwrap_future(std::future<std::any> &fut) -> T {
    auto result = fut.get();
    return std::any_cast<T>(result);
}

TEST(Pool, Simple) {
    auto pool = default_builder().build();

    auto task1_future = pool.emplace([]() { return 1; });
    auto task2_future = pool.emplace([]() { return 2; });
    auto task3_future = pool.emplace([]() { return 3; });

    EXPECT_EQ(unwrap_future<int>(task1_future), 1);
    EXPECT_EQ(unwrap_future<int>(task2_future), 2);
    EXPECT_EQ(unwrap_future<int>(task3_future), 3);
}

TEST(Pool, ResizePool) {
    using namespace std::chrono_literals;

    auto pool = default_builder().build();

    auto task1_future = pool.emplace([]() { return 1; });
    auto task2_future = pool.emplace([]() { return 2; });
    auto task3_future = pool.emplace([]() { return 3; });

    pool.resize_workers(4);

    auto task4_future = pool.emplace([]() { return 4; });
    auto task5_future = pool.emplace([]() { return 5; }); // NOLINT
    auto task6_future = pool.emplace([]() { return 6; }); // NOLINT

    pool.resize_workers(1);

    EXPECT_EQ(unwrap_future<int>(task1_future), 1);
    EXPECT_EQ(unwrap_future<int>(task2_future), 2);
    EXPECT_EQ(unwrap_future<int>(task3_future), 3);
    EXPECT_EQ(unwrap_future<int>(task4_future), 4);
    EXPECT_EQ(unwrap_future<int>(task5_future), 5);
    EXPECT_EQ(unwrap_future<int>(task6_future), 6);
}

} // namespace
