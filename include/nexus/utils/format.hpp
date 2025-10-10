#pragma once

#include <format>
#include <type_traits>

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

} // namespace nexus
