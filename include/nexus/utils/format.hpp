#pragma once

#include "nexus/common.hpp"

#include <cstdio>
#include <format>
#include <type_traits>
#include <utility>

namespace nexus {

/**
 * @brief Check type is formattable.
 *
 * @tparam T Target type.
 * @tparam CharT Char type.
 */
template <typename T, typename CharT = char>
concept Formattable = std::is_default_constructible_v<std::formatter<T, CharT>>;

/**
 * @brief Convert object to a formattable variant.
 *
 * @tparam T Object type.
 * @tparam CharT Char type.
 */
template <typename T, typename CharT = char>
    requires(Formattable<T, CharT>)
static auto to_formattable(const T &value) -> const T & {
    return value; // NOLINT
}

/**
 * @brief Convert object to a formattable variant.
 *
 * @tparam T Object type.
 * @tparam CharT Char type.
 */
template <typename T, typename CharT = char>
    requires(!Formattable<T, CharT>)
static auto to_formattable(const T &value) -> const void * {
    return static_cast<const void *>(&value);
}

/**
 * @brief Format print.
 *
 * @tparam Args Arguments type.
 * @param stream Output stream.
 * @param fmt Format string.
 * @param args Format arguments.
 */
template <typename... Args>
auto print(std::FILE *stream, std::format_string<Args...> fmt, Args &&...args)
    -> void {
    std::fprintf( // NOLINT
        stream, "%s",
        std::format(std::move(fmt), std::forward<Args>(args)...).c_str());
}

/**
 * @brief Format print to stdout.
 *
 * @tparam Args Arguments type.
 * @param fmt Format string.
 * @param args Format arguments.
 */
template <typename... Args>
NEXUS_INLINE auto print(std::format_string<Args...> fmt, Args &&...args)
    -> void {
    print(stdout, std::move(fmt), std::forward<Args>(args)...);
}

/**
 * @brief Format print with `\n`.
 *
 * @tparam Args Arguments type.
 * @param stream Output stream.
 * @param fmt Format string.
 * @param args Format arguments.
 */
template <typename... Args>
auto println(std::FILE *stream, std::format_string<Args...> fmt, Args &&...args)
    -> void {
    std::fprintf( // NOLINT
        stream, "%s\n",
        std::format(std::move(fmt), std::forward<Args>(args)...).c_str());
}

/**
 * @brief Format print with `\n` to stdout.
 *
 * @tparam Args Arguments type.
 * @param fmt Format string.
 * @param args Format arguments.
 */
template <typename... Args>
NEXUS_INLINE auto println(std::format_string<Args...> fmt, Args &&...args)
    -> void {
    println(stdout, std::move(fmt), std::forward<Args>(args)...);
}

/**
 * @brief Print `\n` to stream.
 *
 * @param stream Output stream.
 */
NEXUS_INLINE auto println(std::FILE *stream) -> void {
    std::fputc('\n', stream);
}

/**
 * @brief Print `\n` to stdout.
 *
 */
NEXUS_INLINE auto println() -> void { println(stdout); }

} // namespace nexus
