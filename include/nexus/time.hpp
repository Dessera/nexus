/// @file time.hpp
/// @brief Nexus time utilities.
///
/// This module aims to extend `std::chrono` for convenience.

#pragma once

#include <chrono>
#include <tuple>
#include <type_traits>
#include <utility>

namespace nexus::time {

/// @brief Record the execute duration of a function.
///
/// If a function has return value, then `record` will return it as the second
/// element of tuple.
///
/// ## Example:
///
/// ```cpp
/// // With return.
/// auto [time, ret] = nexus::time::record([]() { return 42; });
///
/// // Without return.
/// auto [time] = nexus::time::record([]() { });
///
/// // With arguments.
/// auto [time, ret] =
///     nexus::time::record([](int a, int b) { return a + b; }, 1, 2);
/// ```
template <typename D = std::chrono::milliseconds,
          typename Clock = std::chrono::high_resolution_clock>
auto record(auto &&func, auto &&...args) -> decltype(auto) {
    using Ret = std::invoke_result_t<decltype(func), decltype(args)...>;

    auto start = Clock::now();
    if constexpr (std::is_same_v<Ret, void>) {
        std::forward<decltype(func)>(func)(
            std::forward<decltype(args)>(args)...);
        return std::make_tuple(
            std::chrono::duration_cast<D>(Clock::now() - start));
    } else {
        auto ret = std::forward<decltype(func)>(func)(
            std::forward<decltype(args)>(args)...);
        return std::make_tuple(
            std::chrono::duration_cast<D>(Clock::now() - start),
            std::move(ret));
    }
}

} // namespace nexus::time
