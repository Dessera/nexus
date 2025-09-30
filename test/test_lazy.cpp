#include <nexus/lazy.hpp>

#include <gtest/gtest.h>
#include <memory>

namespace {

TEST(LazyEval, BasicLazyEval) {
    auto sum = nexus::lazy_eval([]() { return 1 + 2; });
    EXPECT_EQ(*sum, 3);
}

TEST(LazyEval, DepsOfLazyEval) {
    auto dep_value = nexus::lazy_eval_rc([]() { return 42; }); // NOLINT
    auto fin_value =
        nexus::lazy_eval([dep_value]() { return **dep_value + 1; });
    EXPECT_EQ(*fin_value, 43);
}

TEST(LazyEval, MultiCallOfLazyEval) {
    auto mulcall = nexus::lazy_eval([]() {
        static int cnt = 0;
        cnt++;
        return cnt;
    });

    EXPECT_EQ(*mulcall, 1);
    EXPECT_EQ(*mulcall, 1);
}

} // namespace
