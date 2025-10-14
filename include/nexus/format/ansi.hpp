/**
 * @file ansi.hpp
 * @author Dessera (dessera@qq.com)
 * @brief ANSI helper for console.
 * @version 0.1.0
 * @date 2025-10-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "nexus/common.hpp"

#include <format>

namespace nexus::format {

/**
 * @brief ANSI console style.
 *
 */
enum TextStyle : uint8_t {
    Reset = 0,
    Bold = 1,
    Faint = 2,
    Italic = 3,
    Underline = 4,
    SlowBlink = 5,
    RapidBlink = 6,
    Reverse = 7,
    Hide = 8,
    Strike = 9,
    Frame = 51,
    Encircle = 52,
    Overline = 53,

    FGBlack = 30,
    FGRed = 31,
    FGGreen = 32,
    FGYellow = 33,
    FGBlue = 34,
    FGMagenta = 35,
    FGCyan = 36,
    FGWhite = 37,
    FGBrightBlack = 90,
    FGBrightRed = 91,
    FGBrightGreen = 92,
    FGBrightYellow = 93,
    FGBrightBlue = 94,
    FGBrightMagenta = 95,
    FGBrightCyan = 96,
    FGBrightWhite = 97,

    BGBlack = 40,
    BGRed = 41,
    BGGreen = 42,
    BGYellow = 43,
    BGBlue = 44,
    BGMagenta = 45,
    BGCyan = 46,
    BGWhite = 47,
    BGBrightBlack = 100,
    BGBrightRed = 101,
    BGBrightGreen = 102,
    BGBrightYellow = 103,
    BGBrightBlue = 104,
    BGBrightMagenta = 105,
    BGBrightCyan = 106,
    BGBrightWhite = 107,
};

/**
 * @brief ANSI fancy print helper.
 *
 * @tparam T Wrapped type.
 */
template <typename T> class ANSI {
  private:
    const T  *_inner;
    TextStyle _style;

  public:
    constexpr ANSI(const T &value, TextStyle style)
        : _inner(&value), _style(style) {}

    constexpr ~ANSI() = default;

    NEXUS_COPY_DEFAULT(ANSI);
    NEXUS_MOVE_DEFAULT(ANSI);

    ANSI() = delete;

    /**
     * @brief Get value pointer.
     *
     * @return const T* Value pointer.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto pvalue() const -> const T * {
        return _inner;
    }

    /**
     * @brief Get ANSI style.
     *
     * @return TextStyle ANSI style.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto style() const -> TextStyle {
        return _style;
    }
};

/**
 * @brief Void specialization for ANSI, only outputs the command itself.
 *
 */
template <> class ANSI<void> {
  private:
    TextStyle _style;

  public:
    constexpr explicit ANSI(TextStyle style) : _style(style) {}

    constexpr ~ANSI() = default;

    NEXUS_COPY_DEFAULT(ANSI);
    NEXUS_MOVE_DEFAULT(ANSI);

    ANSI() = delete;

    [[nodiscard]] NEXUS_INLINE constexpr auto style() const -> TextStyle {
        return _style;
    }
};

} // namespace nexus::format

template <typename T, typename CharT>
struct std::formatter<nexus::format::ANSI<T>, CharT> {
    using TextStyle = nexus::format::TextStyle;

    constexpr auto parse(auto &ctx) { return ctx.begin(); }

    constexpr auto format(const auto &ansi, auto &ctx) const {
        return std::format_to(ctx.out(), "\033[{}m{}\033[{}m",
                              static_cast<int>(ansi.style()), *ansi.pvalue(),
                              static_cast<int>(TextStyle::Reset));
    }
};

template <typename CharT>
struct std::formatter<nexus::format::ANSI<void>, CharT> {
    using TextStyle = nexus::format::TextStyle;

    constexpr auto parse(auto &ctx) { return ctx.begin(); }

    constexpr auto format(const auto &ansi, auto &ctx) const {
        return std::format_to(ctx.out(), "\033[{}m",
                              static_cast<int>(ansi.style()));
    }
};
