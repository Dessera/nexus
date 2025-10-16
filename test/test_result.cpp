#include "nexus/error.hpp"
#include "nexus/result.hpp"

#include <gtest/gtest.h>
#include <string_view>

namespace {

using nexus::Result;

TEST(Result, Iterator) {
    Result<int, const char *> res = Ok(1);
    int                       flag = 0;
    for ([[maybe_unused]] auto &value : res) {
        ++flag;
    }
    EXPECT_EQ(flag, 1);

    res = Err("Unexpected");
    flag = 0;
    for ([[maybe_unused]] auto &err : res.error_enumerator()) {
        ++flag;
    }
    EXPECT_EQ(flag, 1);
}

TEST(Result, Merge) {
    auto get_ok = [](auto &&) -> Result<int, const char *> { return Ok(2); };

    Result<int, const char *> res1 = Ok(1);
    Result<int, const char *> res2 = Ok(2);
    EXPECT_EQ(res1.both(std::move(res2)).unwrap(), 2); // NOLINT

    res1 = Err("Unexpected");
    res2 = Ok(2);
    EXPECT_EQ(res1.both(std::move(res2)).unwrap_err(), // NOLINT
              std::string_view("Unexpected"));

    res1 = Ok(1);
    res2 = Ok(2);
    EXPECT_EQ(res1.either(std::move(res2)).unwrap(), // NOLINT
              1);

    res1 = Err("Unexpected");
    res2 = Ok(2);
    EXPECT_EQ(res1.either(std::move(res2)).unwrap(), // NOLINT
              2);

    res1 = Ok(1);
    EXPECT_EQ(res1.both_and(get_ok).unwrap(), 2);

    res1 = Err("Unexpected");
    EXPECT_EQ(res1.both_and(get_ok).unwrap_err(),
              std::string_view("Unexpected"));

    res1 = Ok(1);
    EXPECT_EQ(res1.either_or(get_ok).unwrap(), 1);

    res1 = Err("Unexpected");
    EXPECT_EQ(res1.either_or(get_ok).unwrap(), 2);
}

TEST(Result, Flattern) {
    Result<Result<int, const char *>, const char *> res =
        Ok(Result<int, const char *>(Ok(1)));
    EXPECT_EQ(res.flattern().unwrap(), 1);
}

TEST(Result, Inspect) {
    Result<int, const char *> res = Ok(1);
    int                       flag = 0;
    res = res.inspect([&](const int & /*value*/) { ++flag; })
              .inspect_err([&](const auto & /*err*/) { ++flag; });
    EXPECT_EQ(flag, 1);

    res = Err("Unexpected");
    flag = 0;
    res = res.inspect([&](const int & /*value*/) { ++flag; })
              .inspect_err([&](const auto & /*err*/) { ++flag; });
    EXPECT_EQ(flag, 1);
}

TEST(Result, Checkers) {
    Result<int, const char *> res = Ok(1);
    EXPECT_TRUE(res.is_ok());
    EXPECT_FALSE(res.is_err());
    EXPECT_FALSE(res.is_ok_and([](auto && /*value*/) { return false; }));

    res = Ok(1);
    EXPECT_FALSE(res.is_err_and([](auto && /*err*/) { return true; }));

    res = Err("Unexpected");
    EXPECT_FALSE(res.is_ok());
    EXPECT_TRUE(res.is_err());
    EXPECT_FALSE(res.is_ok_and([](auto && /*value*/) { return true; }));

    res = Err("Unexpected");
    EXPECT_FALSE(res.is_err_and([](auto && /*err*/) { return false; }));
}

TEST(Result, Expect) {
    Result<int, const char *> res = Ok(1);
    EXPECT_NO_THROW([[maybe_unused]] auto value = res.expect("Unexpected"));

    res = Ok(1);
    EXPECT_THROW([[maybe_unused]] auto value = res.expect_err("Unexpected"),
                 nexus::ThrowableError);

    res = Err("Unexpected");
    EXPECT_THROW([[maybe_unused]] auto value = res.expect("Unexpected"),
                 nexus::ThrowableError);

    res = Err("Unexpected");
    EXPECT_NO_THROW([[maybe_unused]] auto value = res.expect_err("Unexpected"));
}

TEST(Result, Unwrap) {
    Result<int, const char *> res = Ok(1);
    EXPECT_EQ(res.unwrap(), 1);

    res = Ok(1);
    EXPECT_THROW([[maybe_unused]] auto value = res.unwrap_err(),
                 nexus::ThrowableError);

    res = Err("Unexpected");
    EXPECT_EQ(std::string_view(res.unwrap_err()), "Unexpected");

    res = Err("Unexpected");
    EXPECT_THROW([[maybe_unused]] auto value = res.unwrap(),
                 nexus::ThrowableError);

    res = Ok(1);
    EXPECT_EQ(res.unwrap_or(2), 1);

    res = Err("Unexpected");
    EXPECT_EQ(res.unwrap_or(2), 2);

    res = Ok(1);
    EXPECT_EQ(res.unwrap_or_default(), 1);

    res = Err("Unexpected");
    EXPECT_EQ(res.unwrap_or_default(), 0);
}

TEST(Result, Map) {
    Result<int, const char *> res = Ok(1);

    auto value = res.map([](int value) { return value * 2L; })
                     .map_err([](auto && /*err*/) { return "Expected"; })
                     .unwrap();
    EXPECT_EQ(value, 2);

    res = Err("Unexpected");
    const auto *err = res.map([](int value) { return value * 2L; })
                          .map_err([](auto && /*err*/) { return "Expected"; })
                          .unwrap_err();
    EXPECT_EQ(std::string_view(err), "Expected");

    res = Ok(1);
    value = res.map_or(4, [](int value) { return value * 2L; });
    EXPECT_EQ(value, 2);

    res = Err("Unexpected");
    value = res.map_or(4, [](int value) { return value * 2L; });
    EXPECT_EQ(value, 4);

    res = Ok(1);
    value = res.map_or_default([](int value) { return value * 2L; });
    EXPECT_EQ(value, 2);

    res = Err("Unexpected");
    value = res.map_or_default([](int value) { return value * 2L; });
    EXPECT_EQ(value, 0);

    res = Ok(1);
    value = res.map_or_else([](auto && /*err*/) { return 4; },
                            [](int value) { return value * 2L; });
    EXPECT_EQ(value, 2);

    res = Err("Unexpected");
    value = res.map_or_else([](auto && /*err*/) { return 4; },
                            [](int value) { return value * 2L; });
    EXPECT_EQ(value, 4);
}

} // namespace
