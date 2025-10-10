#pragma once

#include "nexus/common.hpp"

#include <type_traits>
#include <utility>

namespace nexus {

template <typename T>
concept IsOk = requires { typename T::ValueType; };

template <typename T> struct Ok {
  public:
    using ValueType = std::decay_t<T>;

  private:
    ValueType _value;

  public:
    constexpr explicit Ok(ValueType &&value) : _value(std::move(value)) {}
    constexpr explicit Ok(const ValueType &value) : _value(value) {}

    constexpr ~Ok() = default;

    NEXUS_COPY_DEFAULT(Ok);
    NEXUS_MOVE_DEFAULT(Ok);

    constexpr auto value() -> ValueType & { return _value; }

    constexpr auto value() const -> const ValueType & { return _value; }
};

template <typename T> Ok(T value) -> Ok<T>;

template <typename T>
concept IsErr = requires { typename T::ErrorType; };

template <typename E> struct Err {
  public:
    using ErrorType = std::decay_t<E>;

  private:
    ErrorType _error;

  public:
    constexpr explicit Err(ErrorType &&error) : _error(std::move(error)) {}
    constexpr explicit Err(const ErrorType &error) : _error(error) {}

    constexpr ~Err() = default;

    NEXUS_COPY_DEFAULT(Err);
    NEXUS_MOVE_DEFAULT(Err);

    constexpr auto error() -> ErrorType & { return _error; }

    constexpr auto error() const -> const ErrorType & { return _error; }
};

template <typename E> Err(E err) -> Err<E>;

} // namespace nexus
