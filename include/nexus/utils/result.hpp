#pragma once

#include "nexus/common.hpp"
#include "nexus/error.hpp"
#include "nexus/utils/format.hpp"

#include <concepts>
#include <cstddef>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

namespace nexus {

/**
 * @brief Check if type is a Ok.
 *
 * @tparam T Target type.
 * @note The concept only checks types defined in target.
 */
template <typename T>
concept IsOk = requires { typename T::ValueType; };

/**
 * @brief Result value type wrapper.
 *
 * @tparam T Value type.
 */
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

    Ok() = delete;

    /**
     * @brief Get value.
     *
     * @return ValueType& Value reference.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto value() -> ValueType & {
        return _value;
    }

    /**
     * @brief Get value.
     *
     * @return const ValueType& Value reference.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto value() const
        -> const ValueType & {
        return _value;
    }
};

template <typename T> Ok(T value) -> Ok<T>;

/**
 * @brief Check if type is a Err.
 *
 * @tparam T Target type.
 * @note The concept only checks types defined in target.
 */
template <typename T>
concept IsErr = requires { typename T::ErrorType; };

/**
 * @brief Result error type wrapper.
 *
 * @tparam E Error type.
 */
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

    Err() = delete;

    /**
     * @brief Get error.
     *
     * @return ErrorType& Error reference.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto error() -> ErrorType & {
        return _error;
    }

    /**
     * @brief Get error.
     *
     * @return const ErrorType& Error reference.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto error() const
        -> const ErrorType & {
        return _error;
    }
};

template <typename E> Err(E err) -> Err<E>;

/**
 * @brief Check if type is a result.
 *
 * @tparam T Target type.
 * @note The concept only checks types defined in target.
 */
template <typename T>
concept IsResult = requires {
    typename T::ValueType;
    typename T::ErrorType;
    typename T::VariantType;
};

/**
 * @brief A rust-like result type, do not thread safe.
 *
 * @tparam T Value type.
 * @tparam E Error type.
 */
template <typename T, typename E>
class Result : public std::variant<Ok<T>, Err<E>> {
  public:
    /**
     * @brief Value type wrapped in result.
     *
     */
    using ValueType = Ok<T>::ValueType;

    /**
     * @brief Error type wrapped in result.
     *
     */
    using ErrorType = Err<E>::ErrorType;

    /**
     * @brief Result variant type.
     *
     */
    using VariantType = std::variant<Ok<T>, Err<E>>;

    /**
     * @brief Result iterator, will yield one value if the result is not
     * error, otherwise nothing.
     *
     */
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

        /**
         * @brief Unwrap the value (to a reference, do not consuming the
         * result).
         *
         * @return ValueType& Result value.
         */
        [[nodiscard]] NEXUS_INLINE constexpr auto operator*() const
            -> ValueType & {
            return _result->unwrap_ref();
        }

        /**
         * @brief Move iterator to the end.
         *
         * @return Iterator& End iterator.
         */
        constexpr auto operator++() -> Iterator & {
            _result = nullptr;
            return *this;
        }

        /**
         * @brief Move iterator to the end.
         *
         * @return Iterator Previous iterator.
         */
        constexpr auto operator++(int) -> Iterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        /**
         * @brief Check if iterator equals to other.
         *
         * @param other Another iterator.
         * @return true Iterators have the same result pointer.
         * @return false Iterators don't have the same result pointer.
         */
        [[nodiscard]] NEXUS_INLINE constexpr auto
        operator==(const Iterator &other) const -> bool {
            return _result == other._result;
        }
    };

    /**
     * @brief Result error iterator, will yield one error if the result is
     * error, otherwise nothing.
     *
     */
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

        /**
         * @brief Unwrap the error (to a reference, do not consuming the
         * result).
         *
         * @return ErrorType& Result error.
         */
        [[nodiscard]] NEXUS_INLINE constexpr auto operator*() const
            -> ErrorType & {
            return _result->unwrap_err_ref();
        }

        /**
         * @brief Move iterator to the end.
         *
         * @return ErrorIterator& End iterator.
         */
        constexpr auto operator++() -> ErrorIterator & {
            _result = nullptr;
            return *this;
        }

        /**
         * @brief Move iterator to the end.
         *
         * @return ErrorIterator Previous iterator.
         */
        constexpr auto operator++(int) -> ErrorIterator {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        /**
         * @brief Check if iterator equals to other.
         *
         * @param other Another iterator.
         * @return true Iterators have the same result pointer.
         * @return false Iterators don't have the same result pointer.
         */
        [[nodiscard]] NEXUS_INLINE constexpr auto
        operator==(const ErrorIterator &other) const -> bool {
            return _result == other._result;
        }
    };

    /**
     * @brief Enumerate helper for error type.
     *
     */
    class ErrorEnumerator {
      private:
        Result *_result;

      public:
        constexpr explicit ErrorEnumerator(Result &result) : _result(&result) {}

        constexpr ~ErrorEnumerator() = default;

        NEXUS_COPY_DEFAULT(ErrorEnumerator);
        NEXUS_MOVE_DEFAULT(ErrorEnumerator);

        ErrorEnumerator() = delete;

        /**
         * @brief Get the begin iterator.
         *
         * @return ErrorIterator Begin iterator.
         */
        [[nodiscard]] NEXUS_INLINE constexpr auto begin() -> ErrorIterator {
            return _result->ebegin();
        }

        /**
         * @brief Get the end iterator.
         *
         * @return ErrorIterator End iterator.
         */
        [[nodiscard]] NEXUS_INLINE constexpr auto end() -> ErrorIterator {
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

    Result() = delete;

    /**
     * @brief Get the begin iterator.
     *
     * @return Iterator Begin iterator.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto begin() -> Iterator {
        return Iterator(*this);
    }

    /**
     * @brief Get the end iterator.
     *
     * @return Iterator End iterator.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto end() -> Iterator {
        return Iterator();
    }

    /**
     * @brief Get the begin error iterator.
     *
     * @return ErrorIterator Begin error iterator.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto ebegin() -> ErrorIterator {
        return ErrorIterator(*this);
    }

    /**
     * @brief Get the end error iterator.
     *
     * @return ErrorIterator End error iterator.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto eend() -> ErrorIterator {
        return ErrorIterator();
    }

    /**
     * @brief Get the error enumerator.
     *
     * @return ErrorEnumerator error enumerator.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto error_enumerator()
        -> ErrorEnumerator {
        return ErrorEnumerator(*this);
    }

    /**
     * @brief  Return new result if current result is not an error, otherwise
     * return current error.
     *
     * @tparam Tn Another value type.
     * @param res Another result.
     * @return Result<Tn, E> Final result.
     */
    template <typename Tn>
    [[nodiscard]] constexpr auto both(Result<Tn, E> &&res) -> Result<Tn, E> {
        return std::visit(
            [&](auto &&result) -> Result<Tn, E> {
                using VarType = std::decay_t<decltype(result)>;

                if constexpr (IsErr<VarType>) {
                    return Err(std::move(result.error()));
                } else if constexpr (IsOk<VarType>) {
                    return std::move(res);
                } else {
                    static_assert(false,
                                  "Result has an unexpected variant type");
                }
            },
            _base());
    }

    /**
     * @brief Return conv result if current result is not an error, otherwise
     * return current error.
     *
     * @tparam F Value convert function type.
     * @tparam Ret Final result type.
     */
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

    /**
     * @brief Return new result if current result is an error, otherwise return
     * current value.
     *
     * @tparam En Another error type.
     * @param res Another result.
     * @return Result<T, En> Final result.
     */
    template <typename En>
    [[nodiscard]] constexpr auto either(Result<T, En> &&res) -> Result<T, En> {
        return std::visit(
            [&](auto &&result) -> Result<T, En> {
                using VarType = std::decay_t<decltype(result)>;

                if constexpr (IsErr<VarType>) {
                    return std::move(res);
                } else if constexpr (IsOk<VarType>) {
                    return Ok(std::move(result.value()));
                } else {
                    static_assert(false,
                                  "Result has an unexpected variant type");
                }
            },
            _base());
    }

    /**
     * @brief Return conv result if result is an error, otherwise return current
     * value.
     *
     * @tparam F Error convert function type.
     * @tparam Ret Final result type.
     */
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

    /**
     * @brief Convert Result<Result<T, E>, E> to Result<T, E>.
     *
     * @tparam Ret Final result type.
     * @tparam Tn Final result value type.
     * @tparam En Final result error type.
     */
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

    /**
     * @brief Inspect value in result.
     *
     * @param func Inspect function.
     * @return Result Moved result.
     */
    [[nodiscard]] constexpr auto inspect(auto &&func) -> Result {
        for (const auto &value : *this) {
            func(value);
        }

        return std::move(*this);
    }

    /**
     * @brief Inspect error in result.
     *
     * @param func Inspect function.
     * @return Result Moved result.
     */
    [[nodiscard]] constexpr auto inspect_err(auto &&func) -> Result {
        for (const auto &err : this->error_enumerator()) {
            func(err);
        }

        return std::move(*this);
    }

    /**
     * @brief Check if result is error.
     *
     * @return true Result is error.
     * @return false Result is not error.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto is_err() const -> bool {
        return std::get_if<Err<ErrorType>>(&_base()) != nullptr;
    }

    /**
     * @brief Check if result is error and matches predicate.
     *
     * @return true Result is error and matches predicate.
     * @return false Result is not error or does not match predicate.
     */
    [[nodiscard]] constexpr auto is_err_and(auto &&pred) -> bool {
        if (auto *err = std::get_if<Err<ErrorType>>(&_base()); err != nullptr) {
            return pred(std::move(err->error()));
        }
        return false;
    }

    /**
     * @brief Check if result is value.
     *
     * @return true Result is value.
     * @return false Result is not value.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto is_ok() const -> bool {
        return std::get_if<Ok<ValueType>>(&_base()) != nullptr;
    }

    /**
     * @brief Check if result is value and matches predicate.
     *
     * @return true Result is value and matches predicate.
     * @return false Result is not value or does not match predicate.
     */
    [[nodiscard]] constexpr auto is_ok_and(auto &&pred) -> bool {
        if (auto *value = std::get_if<Ok<ValueType>>(&_base());
            value != nullptr) {
            return pred(std::move(value->value()));
        }
        return false;
    }

    /**
     * @brief Get and consume the result, throw if the result is an error.
     *
     * @param msg Error message when throw.
     * @return ValueType Result value.
     * @throw Error Unwrap error when result is an error.
     */
    [[nodiscard]] constexpr auto expect(std::string &&msg) -> ValueType {
        return std::visit(
            [&](auto &&result) -> ValueType {
                using VarType = std::decay_t<decltype(result)>;

                if constexpr (IsErr<VarType>) {
                    throw Error(Error::Unwrap, std::move(msg));
                } else if constexpr (IsOk<VarType>) {
                    return std::move(result.value());
                } else {
                    static_assert(false,
                                  "Result has an unexpected variant type");
                }
            },
            _base());
    }

    /**
     * @brief Get and consume the result, throw if the result is an error.
     *
     * @param msg Error message when throw.
     * @return ValueType Result value.
     * @throw Error Unwrap error when result is an error.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto expect(const std::string &msg)
        -> ValueType {
        expect(std::string(msg));
    }

    /**
     * @brief Get and consume the result, throw if the result is not an error.
     *
     * @param msg Error message when throw.
     * @return ErrorType Result error.
     * @throw Error Unwrap error when result is not an error.
     */
    [[nodiscard]] constexpr auto expect_err(std::string &&msg) -> ErrorType {
        return std::visit(
            [&](auto &&result) -> ErrorType {
                using VarType = std::decay_t<decltype(result)>;

                if constexpr (IsErr<VarType>) {
                    return std::move(result.error());
                } else if constexpr (IsOk<VarType>) {
                    throw Error(Error::Unwrap, std::move(msg));
                } else {
                    static_assert(false,
                                  "Result has an unexpected variant type");
                }
            },
            _base());
    }

    /**
     * @brief Get and consume the result, throw if the result is not an error.
     *
     * @param msg Error message when throw.
     * @return ErrorType Result error.
     * @throw Error Unwrap error when result is not an error.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto expect_err(const std::string &msg)
        -> ErrorType {
        return expect_err(std::string(msg));
    }

    /**
     * @brief Get and consume the result, throw if the result is an error.
     *
     * @return ValueType Result value.
     * @throw Error Unwrap error when result is an error.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto unwrap() -> ValueType {
        return std::move(unwrap_ref());
    }

    /**
     * @brief Get the result value reference, throw if the result is an error.
     *
     * @return ValueType& Result value.
     * @throw Error Unwrap error when result is an error.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto unwrap_ref() -> ValueType & {
        return const_cast<ValueType &>(
            static_cast<const Result *>(this)->unwrap_ref());
    }

    /**
     * @brief Get the result value reference, throw if the result is an error.
     *
     * @return const ValueType& Result value.
     * @throw Error Unwrap error when result is an error.
     */
    [[nodiscard]] constexpr auto unwrap_ref() const -> const ValueType & {
        return std::visit(
            [](auto &&result) -> const ValueType & {
                using VarType = std::decay_t<decltype(result)>;

                if constexpr (IsErr<VarType>) {
                    throw Error(Error::Unwrap, "Result is an error ({})",
                                to_formattable(result.error()));
                } else if constexpr (IsOk<VarType>) {
                    return result.value();
                } else {
                    static_assert(false,
                                  "Result has an unexpected variant type");
                }
            },
            _base());
    }

    /**
     * @brief Get and consume the result, or return the user-defined one if the
     * result is an error.
     *
     * @param value User-defined value.
     * @return ValueType Result value.
     */
    [[nodiscard]] constexpr auto unwrap_or(ValueType &&value) -> ValueType {
        return std::visit(
            [&](auto &&result) -> ValueType {
                using VarType = std::decay_t<decltype(result)>;

                if constexpr (IsErr<VarType>) {
                    return std::move(value);
                } else if constexpr (IsOk<VarType>) {
                    return std::move(result.value());
                } else {
                    static_assert(false,
                                  "Result has an unexpected variant type");
                }
            },
            _base());
    }

    /**
     * @brief Get and consume the result, or return the user-defined one if the
     * result is an error.
     *
     * @param value User-defined value.
     * @return ValueType Result value.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto unwrap_or(const ValueType &value)
        -> ValueType {
        return unwrap_or(ValueType(value));
    }

    /**
     * @brief Get and consume the result, or return the default one.
     *
     * @return ValueType Result value.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto unwrap_or_default() -> ValueType {
        return unwrap_or(ValueType());
    }

    /**
     * @brief Get and consume the result, throw if the result is not an error.
     *
     * @return ErrorType Result error.
     * @throw Error Unwrap error when result is not an error.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto unwrap_err() -> ErrorType {
        return std::move(unwrap_err_ref());
    }

    /**
     * @brief Get the result error reference, throw if the result is not an
     * error.
     *
     * @return ErrorType& Result error.
     * @throw Error Unwrap error when result is not an error.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto unwrap_err_ref() -> ErrorType & {
        return const_cast<ErrorType &>(
            static_cast<const Result *>(this)->unwrap_err_ref());
    }

    /**
     * @brief Get the result error reference, throw if the result is not an
     * error.
     *
     * @return const ErrorType& Result error.
     * @throw Error Unwrap error when result is not an error.
     */
    [[nodiscard]] constexpr auto unwrap_err_ref() const -> const ErrorType & {
        return std::visit(
            [](auto &&result) -> const ErrorType & {
                using VarType = std::decay_t<decltype(result)>;

                if constexpr (IsErr<VarType>) {
                    return result.error();
                } else if constexpr (IsOk<VarType>) {
                    throw Error(Error::Unwrap, "Result is not an error ({})",
                                to_formattable(result.value()));
                } else {
                    static_assert(false,
                                  "Result has an unexpected variant type");
                }
            },
            _base());
    }

    /**
     * @brief Map value to other, return the new result.
     *
     * @tparam F Value convert function type.
     * @tparam Tn New value type.
     * @tparam Ret Mapped result type.
     * @param conv Value convert function.
     * @return Ret Mapped result.
     */
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

    /**
     * @brief Map error to other, return the new result.
     *
     * @tparam F Error convert function type.
     * @tparam En New error type.
     * @tparam Ret Mapped result type.
     * @param conv Error convert function.
     * @return Ret Mapped result.
     */
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

    /**
     * @brief Map value to other, or return the user-defined one.
     *
     * @tparam U User-defined value type.
     * @tparam F Value convert function type.
     * @tparam Ret Mapped return type.
     * @param value User-defined value.
     * @param conv Value convert function.
     * @return Ret Mapped value.
     */
    template <typename U, typename F,
              typename Ret =
                  std::common_type_t<U, std::invoke_result_t<F, ValueType>>>
    [[nodiscard]] constexpr auto map_or(U &&value, F &&conv) -> Ret {
        return std::visit(
            [&](auto &&result) -> Ret {
                using VarType = std::decay_t<decltype(result)>;

                if constexpr (IsErr<VarType>) {
                    return std::forward<U>(value);
                } else if constexpr (IsOk<VarType>) {
                    return std::forward<F>(conv)(std::move(result.value()));
                } else {
                    static_assert(false,
                                  "Result has an unexpected variant type");
                }
            },
            _base());
    }

    /**
     * @brief Map value to other, or return the default one.
     *
     * @tparam F Value convert function type.
     * @tparam Ret Mapped return type.
     * @param conv Value convert function.
     * @return Ret Mapped value.
     */
    template <typename F, typename Ret = std::invoke_result_t<F, ValueType>>
    [[nodiscard]] NEXUS_INLINE constexpr auto map_or_default(F &&conv) -> Ret {
        return map_or(Ret(), std::forward<F>(conv));
    }

    /**
     * @brief Map value or error to other.
     *
     * @tparam Ef Error convert function type.
     * @tparam F Value convert function type.
     * @tparam Ret Mapped return type.
     * @param econv Error convert function.
     * @param conv Value convert function.
     * @return Ret Mapped value.
     */
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
    /**
     * @brief Get variant base from current result.
     *
     * @return VariantType& Variant base object.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto _base() -> VariantType & {
        return *static_cast<VariantType *>(this);
    }

    /**
     * @brief Get variant base from current result.
     *
     * @return const VariantType& Variant base object.
     */
    [[nodiscard]] NEXUS_INLINE constexpr auto _base() const
        -> const VariantType & {
        return *static_cast<const VariantType *>(this);
    }
};

} // namespace nexus

namespace {

using nexus::Err;

using nexus::Ok;

} // namespace
