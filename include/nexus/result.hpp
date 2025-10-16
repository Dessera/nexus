/// @file result.hpp
/// @brief Rust-like result type.
///
/// This `Result` type is implemented by `std::variant`, which may be slower
/// than `std::variant` or `std::optional`.

#pragma once

#include "nexus/common.hpp"
#include "nexus/error.hpp"
#include "nexus/format.hpp"

#include <concepts>
#include <cstddef>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

namespace nexus {

template <typename T>
concept IsOk = requires { typename T::ValueType; };

template <typename T>
concept IsErr = requires { typename T::ErrorType; };

template <typename T>
concept IsResult = requires {
    typename T::ValueType;
    typename T::ErrorType;
    typename T::VariantType;
};

/// @brief Result value wrapper.
///
/// `Ok` is a helper type for the construction of `Result`.
///
/// ## Example:
///
/// ```cpp
/// Result<int, const char*> res = Ok(1);   // implicit conversion.
/// ```
template <typename T> class Ok {
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

    [[nodiscard, nexus_inline]] constexpr auto value() -> ValueType & {
        return _value;
    }

    [[nodiscard, nexus_inline]] constexpr auto value() const
        -> const ValueType & {
        return _value;
    }
};

template <typename T> Ok(T value) -> Ok<T>;

/// @brief Result error wrapper.
///
/// `Err` is a helper type for the construction of `Result`.
///
/// ## Example:
///
/// ```cpp
/// Result<int, const char*> res = Err("Unexpected");   // implicit conversion.
/// ```
template <typename E> class Err {
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

    [[nodiscard, nexus_inline]] constexpr auto error() -> ErrorType & {
        return _error;
    }

    [[nodiscard, nexus_inline]] constexpr auto error() const
        -> const ErrorType & {
        return _error;
    }
};

template <typename E> Err(E err) -> Err<E>;

/// @brief Rust-like result type.
///
/// This type is implemented by `std::variant` and do not thread safe.
template <typename T, typename E>
class Result : public std::variant<Ok<T>, Err<E>> {
  public:
    using ValueType = Ok<T>::ValueType;
    using ErrorType = Err<E>::ErrorType;
    using VariantType = std::variant<Ok<T>, Err<E>>;

    /// @brief Result value iterator.
    ///
    /// The iterator will yield if the result is not error, otherwise nothing.
    class Iterator {
      public:
        using difference_type = std::ptrdiff_t;
        using value_type = ValueType;

      private:
        Result *_result{nullptr};

      public:
        constexpr Iterator(Result &result)
            : _result(result.is_err() ? nullptr : &result) {}

        constexpr Iterator() = default;
        constexpr ~Iterator() = default;

        NEXUS_COPY_DEFAULT(Iterator);
        NEXUS_MOVE_DEFAULT(Iterator);

        [[nodiscard, nexus_inline]] constexpr auto operator*() const
            -> ValueType & {
            return _result->unwrap_ref();
        }

        constexpr auto operator++() -> Iterator & {
            _result = nullptr;
            return *this;
        }

        constexpr auto operator++(int) -> Iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        [[nodiscard, nexus_inline]] constexpr auto
        operator==(const Iterator &other) const -> bool {
            return _result == other._result;
        }
    };

    /// @brief Result error iterator.
    ///
    /// The iterator will yield if the result is error, otherwise nothing.
    class ErrorIterator {
      public:
        using difference_type = std::ptrdiff_t;
        using value_type = ErrorType;

      private:
        Result *_result{nullptr};

      public:
        constexpr ErrorIterator(Result &result)
            : _result(result.is_ok() ? nullptr : &result) {}

        constexpr ErrorIterator() = default;
        constexpr ~ErrorIterator() = default;

        NEXUS_COPY_DEFAULT(ErrorIterator);
        NEXUS_MOVE_DEFAULT(ErrorIterator);

        [[nodiscard, nexus_inline]] constexpr auto operator*() const
            -> ErrorType & {
            return _result->unwrap_err_ref();
        }

        constexpr auto operator++() -> ErrorIterator & {
            _result = nullptr;
            return *this;
        }

        constexpr auto operator++(int) -> ErrorIterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        [[nodiscard, nexus_inline]] constexpr auto
        operator==(const ErrorIterator &other) const -> bool {
            return _result == other._result;
        }
    };

    /// Enumerate helper for error type.
    class ErrorEnumerator {
      private:
        Result *_result;

      public:
        constexpr explicit ErrorEnumerator(Result &result) : _result(&result) {}

        constexpr ~ErrorEnumerator() = default;

        NEXUS_COPY_DEFAULT(ErrorEnumerator);
        NEXUS_MOVE_DEFAULT(ErrorEnumerator);

        /// Get the begin iterator.
        [[nodiscard, nexus_inline]] constexpr auto begin() -> ErrorIterator {
            return _result->ebegin();
        }

        /// Get the end iterator.
        [[nodiscard, nexus_inline]] constexpr auto end() -> ErrorIterator {
            return _result->eend();
        }
    };

    constexpr Result(Ok<ValueType> &&value) : VariantType(std::move(value)) {}
    constexpr Result(const Ok<ValueType> &value) : VariantType(value) {}

    constexpr Result(Err<ErrorType> &&err) : VariantType(std::move(err)) {}
    constexpr Result(const Err<ErrorType> &err) : VariantType(err) {}

    constexpr ~Result() = default;

    NEXUS_COPY_DEFAULT(Result);
    NEXUS_MOVE_DEFAULT(Result);

    /// Get the begin iterator.
    [[nodiscard, nexus_inline]] constexpr auto begin() -> Iterator {
        return Iterator(*this);
    }

    /// Get the end iterator.
    [[nodiscard, nexus_inline]] constexpr auto end() -> Iterator {
        return Iterator();
    }

    /// Get the begin error iterator.
    [[nodiscard, nexus_inline]] constexpr auto ebegin() -> ErrorIterator {
        return ErrorIterator(*this);
    }

    /// Get the end error iterator.
    [[nodiscard, nexus_inline]] constexpr auto eend() -> ErrorIterator {
        return ErrorIterator();
    }

    /// Get the error enumerator.
    [[nodiscard, nexus_inline]] constexpr auto error_enumerator()
        -> ErrorEnumerator {
        return ErrorEnumerator(*this);
    }

    /// @brief Return new result if current result is not an error, otherwise
    /// return current error.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// Result<int, const char *> res1 = Ok(1);
    /// Result<int, const char *> res2 = Ok(2);
    /// assert(res1.both(std::move(res2)).unwrap() == 2);
    /// ```
    template <typename Tn>
    [[nodiscard]] constexpr auto both(Result<Tn, E> &&res) -> Result<Tn, E> {
        if (auto *perr = std::get_if<Err<ErrorType>>(&_base());
            perr != nullptr) {
            return Err(std::move(perr->error()));
        }
        return std::move(res);
    }

    /// Return conv result if current result is not an error, otherwise return
    /// current error.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// Result<int, const char *> res1 = Ok(1);
    /// Result<int, const char *> res2 = res1.both_and(
    ///     [](auto &&) -> Result<int, const char *> { return Ok(2); });
    /// assert(res2.unwrap() == 2);
    /// ```
    template <typename F, typename Ret = std::invoke_result_t<F, ValueType>>
    [[nodiscard]] constexpr auto both_and(F &&conv) -> Ret
        requires(IsResult<Ret>)
    {
        return std::visit(
            [&](auto &&result) -> Ret {
                using VarType = std::decay_t<decltype(result)>;

                if constexpr (IsErr<VarType>) {
                    return Err(std::move(result.error()));
                } else if constexpr (IsOk<VarType>) {
                    return std::forward<F>(conv)(std::move(result.value()));
                } else {
                    static_assert(false,
                                  "Result has an unexpected variant type");
                }
            },
            _base());
    }

    /// @brief Return new result if current result is an error, otherwise return
    /// current value.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// Result<int, const char *> res1 = Ok(1);
    /// Result<int, const char *> res2 = Ok(2);
    /// assert(res1.either(std::move(res2)).unwrap() == 1);
    /// ```
    template <typename En>
    [[nodiscard]] constexpr auto either(Result<T, En> &&res) -> Result<T, En> {
        if (auto *pvalue = std::get_if<Ok<ValueType>>(&_base());
            pvalue != nullptr) {
            return Ok(std::move(pvalue->value()));
        }
        return std::move(res);
    }

    /// @brief Return conv result if result is an error, otherwise return
    /// current value.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// Result<int, const char *> res1 = Ok(1);
    /// Result<int, const char *> res2 = res1.either_or(
    ///     [](auto &&) -> Result<int, const char *> { return Ok(2); });
    /// assert(res2.unwrap() == 1);
    /// ```
    template <typename F, typename Ret = std::invoke_result_t<F, ErrorType>>
    [[nodiscard]] constexpr auto either_or(F &&conv) -> Ret
        requires(IsResult<Ret>)
    {
        return std::visit(
            [&](auto &&result) -> Ret {
                using VarType = std::decay_t<decltype(result)>;

                if constexpr (IsErr<VarType>) {
                    return std::forward<F>(conv)(std::move(result.error()));
                } else if constexpr (IsOk<VarType>) {
                    return Ok(std::move(result.value()));
                } else {
                    static_assert(false,
                                  "Result has an unexpected variant type");
                }
            },
            _base());
    }

    /// @brief Convert Result<Result<T, E>, E> to Result<T, E>.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// Result<Result<int, const char *>, const char *> res =
    ///     Ok(Result<int, const char *>(Ok(1)));
    /// assert(res.flattern().unwrap() == 1);
    /// ```
    template <typename Ret = ValueType, typename Tn = Ret::ValueType,
              typename En = Ret::ErrorType>
    [[nodiscard]] constexpr auto flattern() -> Ret
        requires(std::same_as<Ret, Result<Tn, En>>)
    {
        return std::visit(
            [](auto &&result) -> Ret {
                using VarType = std::decay_t<decltype(result)>;

                if constexpr (IsErr<VarType>) {
                    return Err(std::move(result.error()));
                } else if constexpr (IsOk<VarType>) {
                    return std::move(result.value());
                } else {
                    static_assert(false,
                                  "Result has an unexpected variant type");
                }
            },
            _base());
    }

    /// @brief Inspect value in result.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// Result<int, const char *> res = Ok(1);
    /// res =
    ///     res.inspect([&](const int &value) { nexus::println("{}", value); });
    /// ```
    [[nodiscard]] constexpr auto inspect(auto &&func) -> Result {
        if (auto *pvalue = std::get_if<Ok<ValueType>>(&_base());
            pvalue != nullptr) {
            std::forward<decltype(func)>(func)(pvalue->value());
        }

        return std::move(*this);
    }

    /// @brief Inspect error in result.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// Result<int, const char *> res = Err("Unexpected");
    /// res = res.inspect_err(
    ///     [&](const auto &err) { nexus::println("{}", err); });
    /// ```
    [[nodiscard]] constexpr auto inspect_err(auto &&func) -> Result {
        if (auto *perr = std::get_if<Err<ErrorType>>(&_base());
            perr != nullptr) {
            std::forward<decltype(func)>(func)(perr->error());
        }

        return std::move(*this);
    }

    /// Check if result is error.
    [[nodiscard, nexus_inline]] constexpr auto is_err() const -> bool {
        return std::get_if<Err<ErrorType>>(&_base()) != nullptr;
    }

    /// Check if result is error and matches predicate.
    [[nodiscard]] constexpr auto is_err_and(auto &&pred) -> bool {
        if (auto *perr = std::get_if<Err<ErrorType>>(&_base());
            perr != nullptr) {
            return std::forward<decltype(pred)>(pred)(std::move(perr->error()));
        }
        return false;
    }

    /// Check if result is value.
    [[nodiscard, nexus_inline]] constexpr auto is_ok() const -> bool {
        return std::get_if<Ok<ValueType>>(&_base()) != nullptr;
    }

    /// Check if result is value and matches predicate.
    [[nodiscard]] constexpr auto is_ok_and(auto &&pred) -> bool {
        if (auto *pvalue = std::get_if<Ok<ValueType>>(&_base());
            pvalue != nullptr) {
            return std::forward<decltype(pred)>(pred)(
                std::move(pvalue->value()));
        }
        return false;
    }

    /// @brief Get the value, throw specific message if there is an error.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// Result<int, const char *> res = Err("Unexpected");
    /// auto value = res.expect("Unexpected");    // throw here
    /// ```
    [[nodiscard]] constexpr auto expect(std::string &&msg) -> ValueType {
        if (auto *pvalue = std::get_if<Ok<ValueType>>(&_base());
            pvalue != nullptr) {
            return std::move(pvalue->value());
        }

        throw Error(Error::Unwrap, std::move(msg)).to_std();
    }

    /// Get the value, throw specific message if there is an error.
    [[nodiscard, nexus_inline]] constexpr auto expect(const std::string &msg)
        -> ValueType {
        expect(std::string(msg));
    }

    /// @brief Get the error, throw specific message if there is no error.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// Result<int, const char *> res = Ok(1);
    /// auto err = res.expect_err("Unexpected");    // throw here
    /// ```
    [[nodiscard]] constexpr auto expect_err(std::string &&msg) -> ErrorType {
        if (auto *perr = std::get_if<Err<ErrorType>>(&_base());
            perr != nullptr) {
            return std::move(perr->error());
        }

        throw Error(Error::Unwrap, std::move(msg)).to_std();
    }

    /// Get the error, throw specific message if there is no error.
    [[nodiscard, nexus_inline]] constexpr auto
    expect_err(const std::string &msg) -> ErrorType {
        return expect_err(std::string(msg));
    }

    /// @brief Get the value, throw if there is an error.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// Result<int, const char *> res = Ok(1);
    /// assert(res.unwrap() == 1);
    /// ```
    [[nodiscard, nexus_inline]] constexpr auto unwrap() -> ValueType {
        return std::move(unwrap_ref());
    }

    /// Get the reference of value, throw if there is an error.
    [[nodiscard, nexus_inline]] constexpr auto unwrap_ref() -> ValueType & {
        return const_cast<ValueType &>(
            static_cast<const Result *>(this)->unwrap_ref());
    }

    /// Get the reference of value, throw if there is an error.
    [[nodiscard]] constexpr auto unwrap_ref() const -> const ValueType & {
        return std::visit(
            [](auto &&result) -> const ValueType & {
                using VarType = std::decay_t<decltype(result)>;

                if constexpr (IsErr<VarType>) {
                    throw Error(Error::Unwrap, "Result is an error ({})",
                                to_formattable(result.error()))
                        .to_std();
                } else if constexpr (IsOk<VarType>) {
                    return result.value();
                } else {
                    static_assert(false,
                                  "Result has an unexpected variant type");
                }
            },
            _base());
    }

    /// @brief Get the value, or return the user-defined one.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// Result<int, const char *> res = Err("Unexpected");
    /// auto value = res.unwrap_or(0);
    /// assert(value == 0);
    /// ```
    [[nodiscard]] constexpr auto unwrap_or(ValueType &&value) -> ValueType {
        if (auto *pvalue = std::get_if<Ok<ValueType>>(&_base());
            pvalue != nullptr) {
            return std::move(pvalue->value());
        }

        return std::move(value);
    }

    /// Get the value, or return the user-defined one.
    [[nodiscard, nexus_inline]] constexpr auto unwrap_or(const ValueType &value)
        -> ValueType {
        return unwrap_or(ValueType(value));
    }

    /// @brief Get the value, or return the default one.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// Result<int, const char *> res = Err("Unexpected");
    /// auto value = res.unwrap_or_default();
    /// assert(value == 0);
    /// ```
    [[nodiscard, nexus_inline]] constexpr auto unwrap_or_default()
        -> ValueType {
        return unwrap_or(ValueType());
    }

    /// @brief Get the error, throw if there is no error.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// Result<int, const char *> res = Err("Unexpected");
    /// assert(std::string_view(res.unwrap_err()) == "Unexpected");
    /// ```
    [[nodiscard, nexus_inline]] constexpr auto unwrap_err() -> ErrorType {
        return std::move(unwrap_err_ref());
    }

    /// Get the reference of error, throw if there is no error.
    [[nodiscard, nexus_inline]] constexpr auto unwrap_err_ref() -> ErrorType & {
        return const_cast<ErrorType &>(
            static_cast<const Result *>(this)->unwrap_err_ref());
    }

    /// Get the const reference of error, throw if there is no error.
    [[nodiscard]] constexpr auto unwrap_err_ref() const -> const ErrorType & {
        return std::visit(
            [](auto &&result) -> const ErrorType & {
                using VarType = std::decay_t<decltype(result)>;

                if constexpr (IsErr<VarType>) {
                    return result.error();
                } else if constexpr (IsOk<VarType>) {
                    throw Error(Error::Unwrap, "Result is not an error ({})",
                                to_formattable(result.value()))
                        .to_std();
                } else {
                    static_assert(false,
                                  "Result has an unexpected variant type");
                }
            },
            _base());
    }

    /// @brief Map value, convert it to another type. Returns the new result.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// Result<int, const char *> res = Ok(1);
    /// auto value = res.map([](int value) { return value * 2L; }).unwrap();
    /// assert(value == 2);
    /// ```
    template <typename F, typename Tn = std::invoke_result_t<F, ValueType>,
              typename Ret = Result<Tn, E>>
    [[nodiscard]] constexpr auto map(F &&conv) -> Ret {
        return std::visit(
            [&](auto &&result) -> Ret {
                using VarType = std::decay_t<decltype(result)>;

                if constexpr (IsErr<VarType>) {
                    return Err(std::move(result.error()));
                } else if constexpr (IsOk<VarType>) {
                    return Ok(std::forward<F>(conv)(std::move(result.value())));
                } else {
                    static_assert(false,
                                  "Result has an unexpected variant type");
                }
            },
            _base());
    }

    /// @brief Map error, convert it to another type. Returns the new result.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// Result<int, const char *> res = Err("Unexpected");
    /// auto err = res.map_err([](auto &&e) { return std::string_view(e); })
    ///                .unwrap_err();
    /// assert(err == "Unexpected");
    /// ```
    template <typename F, typename En = std::invoke_result_t<F, ErrorType>,
              typename Ret = Result<T, En>>
    [[nodiscard]] constexpr auto map_err(F &&conv) -> Ret {
        return std::visit(
            [&](auto &&result) -> Ret {
                using VarType = std::decay_t<decltype(result)>;

                if constexpr (IsErr<VarType>) {
                    return Err(
                        std::forward<F>(conv)(std::move(result.error())));
                } else if constexpr (IsOk<VarType>) {
                    return Ok(std::move(result.value()));
                } else {
                    static_assert(false,
                                  "Result has an unexpected variant type");
                }
            },
            _base());
    }

    /// @brief Map value, convert it to another type, or return the user-defined
    /// one.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// // Ok case:
    /// Result<int, const char *> res = Ok(1);
    /// value = res.map_or(0, [](int value) { return value * 2L; });
    /// assert(value == 2);
    ///
    /// // Err case:
    /// res = Err("Unexpected");
    /// value = res.map_or(0, [](int value) { return value * 2L; });
    /// assert(value == 0);
    /// ```
    template <typename U, typename F,
              typename Ret =
                  std::common_type_t<U, std::invoke_result_t<F, ValueType>>>
    [[nodiscard]] constexpr auto map_or(U &&value, F &&conv) -> Ret {
        if (auto *pvalue = std::get_if<Ok<ValueType>>(&_base());
            pvalue != nullptr) {
            return std::forward<F>(conv)(std::move(pvalue->value()));
        }

        return std::forward<U>(value);
    }

    /// @brief Map value, convert it to another type, or return the default one.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// // Ok case:
    /// Result<int, const char *> res = Ok(1);
    /// value = res.map_or_default([](int value) { return value * 2L; });
    /// assert(value == 2);
    ///
    /// // Err case:
    /// res = Err("Unexpected");
    /// value = res.map_or_default([](int value) { return value * 2L; });
    /// assert(value == 0);
    /// ```
    template <typename F, typename Ret = std::invoke_result_t<F, ValueType>>
    [[nodiscard, nexus_inline]] constexpr auto map_or_default(F &&conv) -> Ret {
        return map_or(Ret(), std::forward<F>(conv));
    }

    /// @brief Map value or error, convert them to another type.
    ///
    /// ## Example:
    ///
    /// ```cpp
    /// // Ok case:
    /// Result<int, const char *> res = Ok(1);
    /// value = res.map_or_else([](auto &&) { return 4; },
    ///                         [](int value) { return value * 2L; });
    /// assert(value == 2);
    ///
    /// // Err case:
    /// res = Err("Unexpected");
    /// value = res.map_or_else([](auto &&) { return 4; },
    ///                         [](int value) { return value * 2L; });
    /// assert(value == 4);
    /// ```
    template <
        typename Ef, typename F,
        typename Ret = std::common_type_t<std::invoke_result_t<Ef, ErrorType>,
                                          std::invoke_result_t<F, ValueType>>>
    [[nodiscard]] constexpr auto map_or_else(Ef &&econv, F &&conv) -> Ret {
        return std::visit(
            [&](auto &&result) -> Ret {
                using VarType = std::decay_t<decltype(result)>;

                if constexpr (IsErr<VarType>) {
                    return std::forward<Ef>(econv)(std::move(result.error()));
                } else if constexpr (IsOk<VarType>) {
                    return std::forward<F>(conv)(std::move(result.value()));
                } else {
                    static_assert(false,
                                  "Result has an unexpected variant type");
                }
            },
            _base());
    }

  private:
    /// Get variant base from current result.
    [[nodiscard, nexus_inline]] constexpr auto _base() -> VariantType & {
        return *static_cast<VariantType *>(this);
    }

    /// Get variant base from current result.
    [[nodiscard, nexus_inline]] constexpr auto _base() const
        -> const VariantType & {
        return *static_cast<const VariantType *>(this);
    }
};

} // namespace nexus

using nexus::Err;

using nexus::Ok;
