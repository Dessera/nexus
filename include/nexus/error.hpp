/// @file error.hpp
/// Nexus error definition.

#pragma once

#include "nexus/common.hpp"

#include <cstdint>
#include <cstring>
#include <exception>
#include <format>
#include <string>
#include <utility>

namespace nexus {

class ThrowableError;

/// Nexus error type.
class NEXUS_EXPORT Error {
  public:
    /// Error codes definition.
    enum Code : std::uint8_t {
        Unwrap, ///< Reserved by Result<T, E>.
    };

  private:
    std::string _msg;
    Code        _code;

  public:
    constexpr Error(Code code, std::string &&msg)
        : _msg(std::move(msg)), _code(code) {}

    constexpr Error(Code code, const std::string &msg)
        : Error(code, std::string(msg)) {}

    constexpr Error(Code code, const char *msg) : _msg(msg), _code(code) {}

    explicit Error(Code code) : Error(code, std::strerror(errno)) {}

    template <typename... Args>
    Error(Code code, std::format_string<Args...> fmt, Args &&...args)
        : Error(code, std::format(fmt, std::forward<Args>(args)...)) {}

    constexpr ~Error() = default;

    NEXUS_COPY_DEFAULT(Error);
    NEXUS_MOVE_DEFAULT(Error);

    /// Get error code.
    [[nodiscard, nexus_inline]] constexpr auto code() const -> Code {
        return _code;
    }

    /// Get error message.
    [[nodiscard, nexus_inline]] constexpr auto msg() const
        -> const std::string & {
        return _msg;
    }

    /// @brief Convert current error to a standard error.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// throw Error(Error::Unwrap, "Some error").to_std();
    /// ```
    [[nodiscard]] auto to_std() -> ThrowableError;
};

/// Error wrapper for standard compatible.
class NEXUS_EXPORT ThrowableError : std::exception {
  private:
    Error _err;

  public:
    ThrowableError(Error &&err) : _err(std::move(err)) {}

    ~ThrowableError() override = default;

    NEXUS_COPY_DEFAULT(ThrowableError);
    NEXUS_MOVE_DEFAULT(ThrowableError);

    [[nodiscard]] auto what() const noexcept -> const char * override;
};

} // namespace nexus
