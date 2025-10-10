#pragma once

#include "nexus/common.hpp"

#include <cstdint>
#include <cstring>
#include <exception>
#include <format>
#include <string>
#include <utility>

namespace nexus {

/**
 * @brief Nexus error type.
 *
 */
class NEXUS_EXPORT Error : public std::exception {
  public:
    /**
     * @brief Error codes definition.
     *
     */
    enum Code : std::uint8_t {
        Unwrap, /**< Reserved by Result<T, E> */
    };

  private:
    std::string _msg;
    Code        _code;

  public:
    Error(Code code, std::string &&msg);
    Error(Code code, const std::string &msg);
    Error(Code code, const char *msg);

    explicit Error(Code code);

    template <typename... Args>
    Error(Code code, std::format_string<Args...> fmt, Args &&...args)
        : Error(code, std::format(fmt, std::forward<Args>(args)...)) {}

    ~Error() override = default;

    NEXUS_COPY_DEFAULT(Error);
    NEXUS_MOVE_DEFAULT(Error);

    [[nodiscard]] auto what() const noexcept -> const char * override;

    /**
     * @brief Get error code.
     *
     * @return Code Error code.
     */
    [[nodiscard]] NEXUS_INLINE auto code() const -> Code { return _code; }

    /**
     * @brief Get error message.
     *
     * @return const std::string& Message.
     */
    [[nodiscard]] NEXUS_INLINE auto msg() const -> const std::string & {
        return _msg;
    }
};

} // namespace nexus
