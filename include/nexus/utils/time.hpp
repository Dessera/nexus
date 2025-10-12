/**
 * @file time.hpp
 * @author Dessera (dessera@qq.com)
 * @brief Nexus time utilities.
 * @version 0.1.0
 * @date 2025-10-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include <chrono>
#include <concepts>
#include <tuple>
#include <type_traits>
#include <utility>

namespace nexus::time {

namespace chrono = std::chrono;

/**
 * @brief Record the execute duration of a function.
 *
 * @tparam D Duration type.
 * @tparam Clock Clock type.
 */
template <typename D = chrono::milliseconds,
          typename Clock = chrono::high_resolution_clock>
auto record(auto &&func, auto &&...args) -> decltype(auto)
    requires(!std::same_as<
             std::invoke_result_t<decltype(func), decltype(args)...>, void>)
{
    auto start = Clock::now();
    auto ret = std::forward<decltype(func)>(func)(
        std::forward<decltype(args)>(args)...);
    auto end = Clock::now();

    return std::make_tuple(chrono::duration_cast<D>(end - start),
                           std::move(ret));
}

/**
 * @brief Record the execute duration of a function.
 *
 * @tparam D Duration type.
 * @tparam Clock Clock type.
 */
template <typename D = chrono::milliseconds,
          typename Clock = chrono::high_resolution_clock>
auto record(auto &&func, auto &&...args) -> decltype(auto)
    requires(std::same_as<
             std::invoke_result_t<decltype(func), decltype(args)...>, void>)
{
    auto start = Clock::now();
    std::forward<decltype(func)>(func)(std::forward<decltype(args)>(args)...);
    auto end = Clock::now();

    return std::make_tuple(chrono::duration_cast<D>(end - start));
}

} // namespace nexus::time
