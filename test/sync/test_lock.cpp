#include "nexus/sync/mutex.hpp"

#include <gtest/gtest.h>

namespace {

using nexus::sync::Mutex;

TEST(Mutex, Simple) {
    auto value = Mutex<int>(0);
    auto value_handle = value.lock();

    EXPECT_EQ(*value_handle, 0);
}

} // namespace
