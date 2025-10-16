/// @file format.hpp
/// Format utilities.

#pragma once

#include "nexus/common.hpp"
#include "nexus/format/ansi.hpp"

#include <cstdio>
#include <format>
#include <type_traits>
#include <utility>

namespace nexus {

template <typename T, typename CharT = char>
concept Formattable = std::is_default_constructible_v<std::formatter<T, CharT>>;

/// @brief Convert object to a formattable variant.
///
/// If object is formattable, then return it's const reference, otherwise return
/// the const void pointer of it.
template <typename T, typename CharT = char>
[[nodiscard, nexus_inline]] constexpr auto to_formattable(const T &value)
    -> decltype(auto) {
    if constexpr (Formattable<T, CharT>) {
        return value;
    } else {
        return static_cast<const void *>(&value);
    }
}

/// @brief Wrap object with ANSI command.
///
/// ## Example:
///
/// ```cpp
/// using namespace nexus;
///
/// print("{}", ansi(42, format::BGRed));   // \033[41m42\033[0m
/// ```
template <typename T>
[[nodiscard, nexus_inline]] constexpr auto ansi(const T          &value,
                                                format::TextStyle style)
    -> format::ANSI<T> {
    return format::ANSI(value, style);
}

/// @brief Get single ANSI command.
///
/// ## Example:
///
/// ```cpp
/// using namespace nexus;
///
/// print("{}", ansi(format::BGRed));   // \033[41m
/// ```
[[nodiscard, nexus_inline]] constexpr auto ansi(format::TextStyle style)
    -> format::ANSI<void> {
    return format::ANSI<void>(style);
}

/// Format print.
template <typename... Args>
auto print(std::FILE *stream, std::format_string<Args...> fmt, Args &&...args)
    -> void {
    std::fprintf( // NOLINT
        stream, "%s",
        std::format(std::move(fmt), std::forward<Args>(args)...).c_str());
}

/// Format print to stdout.
template <typename... Args>
[[nexus_inline]] auto print(std::format_string<Args...> fmt, Args &&...args)
    -> void {
    print(stdout, std::move(fmt), std::forward<Args>(args)...);
}

/// Format print with `\n`.
template <typename... Args>
auto println(std::FILE *stream, std::format_string<Args...> fmt, Args &&...args)
    -> void {
    std::fprintf( // NOLINT
        stream, "%s\n",
        std::format(std::move(fmt), std::forward<Args>(args)...).c_str());
}

/// Format print with `\n` to stdout.
template <typename... Args>
[[nexus_inline]] inline auto println(std::format_string<Args...> fmt,
                                     Args &&...args) -> void {
    println(stdout, std::move(fmt), std::forward<Args>(args)...);
}

/// Print `\n` to stream.
[[nexus_inline]] inline auto println(std::FILE *stream) -> void {
    std::fputc('\n', stream);
}

/// Print `\n` to stdout.
[[nexus_inline]] inline auto println() -> void { println(stdout); }

} // namespace nexus
