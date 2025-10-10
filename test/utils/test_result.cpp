#include "nexus/error.hpp"
#include "nexus/utils/result.hpp"

#include <gtest/gtest.h>
#include <utility>

namespace {

using nexus::Error;
using nexus::Result;

auto make_value(int value) -> Result<int> { return value; }

auto make_error() -> Result<int> {
    return Error(Error::Unwrap, "Unknown error");
}

TEST(Result, Iterator) {
    auto res = make_value(1);
    int  flag = 0;
    for ([[maybe_unused]] auto &value : res) {
        ++flag;
    }
    EXPECT_EQ(flag, 1);

    res = make_error();
    flag = 0;
    for ([[maybe_unused]] auto &err : res.error_enumerator()) {
        ++flag;
    }
    EXPECT_EQ(flag, 1);
}

TEST(Result, Checkers) {
    auto res = make_value(1);
    EXPECT_TRUE(res.is_ok());
    EXPECT_FALSE(res.is_err());
    EXPECT_FALSE(res.is_ok_and([](auto && /*value*/) { return false; }));
    EXPECT_FALSE(res.is_err_and([](auto && /*err*/) { return true; }));

    res = make_error();
    EXPECT_FALSE(res.is_ok());
    EXPECT_TRUE(res.is_err());
    EXPECT_FALSE(res.is_ok_and([](auto && /*value*/) { return true; }));
    EXPECT_FALSE(res.is_err_and([](auto && /*err*/) { return false; }));
}

TEST(Result, Expect) {
    auto res = make_value(1);
    EXPECT_NO_THROW([[maybe_unused]] auto value = res.expect("Unexpected"));

    res = make_value(1);
    EXPECT_THROW([[maybe_unused]] auto value = res.expect_err("Unexpected"),
                 Error);

    res = make_error();
    EXPECT_THROW([[maybe_unused]] auto value = res.expect("Unexpected"), Error);

    res = make_error();
    EXPECT_NO_THROW([[maybe_unused]] auto value = res.expect_err("Unexpected"));
}

TEST(Result, Unwrap) {
    auto res = make_value(1);
    EXPECT_EQ(res.unwrap(), 1);

    res = make_value(1);
    EXPECT_THROW([[maybe_unused]] auto value = res.unwrap_err(), Error);

    res = make_error();
    EXPECT_EQ(res.unwrap_err().code(), Error::Unwrap);

    res = make_error();
    EXPECT_THROW([[maybe_unused]] auto value = res.unwrap(), Error);

    res = make_error();
    EXPECT_EQ(res.unwrap_or(2), 2);

    res = make_error();
    EXPECT_EQ(res.unwrap_or_default(), 0);
}

TEST(Result, Merge) {
    auto res = make_value(1).both(make_value(2));
    EXPECT_EQ(res.unwrap(), 2);

    res = make_error().both(make_value(1));
    EXPECT_THROW([[maybe_unused]] auto value = res.unwrap(), Error);

    res = make_value(1).both_and([](auto &&) { return make_value(2); });
    EXPECT_EQ(res.unwrap(), 2);

    res = make_error().both_and([](auto &&) { return make_value(1); });
    EXPECT_THROW([[maybe_unused]] auto value = res.unwrap(), Error);

    res = make_value(1).either(make_value(2));
    EXPECT_EQ(res.unwrap(), 1);

    res = make_error().either(make_value(1));
    EXPECT_EQ(res.unwrap(), 1);

    res = make_value(1).either_or([](auto &&) { return make_value(2); });
    EXPECT_EQ(res.unwrap(), 1);

    res = make_error().either_or([](auto &&) { return make_value(1); });
    EXPECT_EQ(res.unwrap(), 1);
}

TEST(Result, Flattern) {
    auto res_ok = Result<Result<int>>(1);
    auto res_flat = res_ok.flattern();

    EXPECT_EQ(res_flat.unwrap(), 1);
}

TEST(Result, Inspect) {
    auto res = make_value(1);
    int  flag = 0;
    res = res.inspect([&](const int & /*value*/) { ++flag; })
              .inspect_err([&](const auto & /*err*/) { ++flag; });
    EXPECT_EQ(flag, 1);

    res = make_error();
    flag = 0;
    res = res.inspect([&](const int & /*value*/) { ++flag; })
              .inspect_err([&](const auto & /*err*/) { ++flag; });
    EXPECT_EQ(flag, 1);
}

TEST(Result, Map) {
    auto res = make_value(1).map([](int value) { return value * 2; });
    EXPECT_EQ(res.unwrap(), 2);

    res = make_error().map_err([](Error &&err) { return std::move(err); });
    EXPECT_THROW([[maybe_unused]] auto value = res.unwrap(), Error);

    auto res_value =
        make_error().map_or(3, [](int value) { return value * 2; });
    EXPECT_EQ(res_value, 3);

    res_value = make_error().map_or_else([](auto && /*err*/) { return 3; },
                                         [](int value) { return value * 2; });
    EXPECT_EQ(res_value, 3);
}

} // namespace
