#include <nexus/curried.hpp>

#include <functional>
#include <gtest/gtest.h>

namespace {

TEST(Curried, BasicCurried) {
    auto cadd = nexus::make_curried(std::plus<>());
    EXPECT_EQ(cadd(1)(2), 3);

    auto cadd_five = cadd(5); // NOLINT
    EXPECT_EQ(cadd_five(4), 9);
    EXPECT_EQ(cadd_five(5), 10);
}

} // namespace
