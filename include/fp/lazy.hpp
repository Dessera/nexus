#pragma once

#include <concepts>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>

namespace fp {

/**
 * @brief Trait for return type based lazy evaluator.
 *
 * @tparam R Return type.
 */
template <typename R> struct LazyResult {
    using Type = std::decay_t<R>;

    /**
     * @brief Get const reference to result.
     *
     * @return const Type& Result reference.
     */
    [[nodiscard]] constexpr virtual auto get_cref() const -> const Type & = 0;

    /**
     * @brief Get copied value from result.
     *
     * @return Type Result value.
     */
    [[nodiscard]] constexpr auto get() const -> Type { return get_cref(); }

    /**
     * @brief Alias for `get_cref`, get const reference to result.
     *
     * @return const Type& Result reference.
     */
    [[nodiscard]] constexpr auto operator*() const -> decltype(auto) {
        return get_cref();
    }
};

/**
 * @brief Lazy value evaluator.
 *
 * @tparam F Evaluator function type.
 */
template <typename F>
    requires(std::is_invocable_v<F>)
class LazyEval : public LazyResult<std::invoke_result_t<F>> {
  public:
    /**
     * @brief Decayed evaluator type for storage of function.
     *
     */
    using Evaluator = std::decay_t<F>;

    /**
     * @brief Decayed evaluator result with no reference.
     *
     */
    using Type = LazyResult<std::invoke_result_t<F>>::Type;

  private:
    Evaluator _func;
    mutable Type _result;
    mutable std::once_flag _flag;

  public:
    /**
     * @brief Construct a new lazy evaluator.
     *
     */
    constexpr LazyEval(const Evaluator &func) : _func(func) {}

    /**
     * @brief Construct a new lazy evaluator.
     *
     */
    constexpr LazyEval(Evaluator &&func) : _func(std::move(func)) {}

    [[nodiscard]] constexpr auto get_cref() const -> const Type & override {
        std::call_once(_flag, [this]() { _result = _func(); });
        return _result;
    }
};

/**
 * @brief Construct a new lazy evaluator.
 *
 * @tparam F Evaluator function type.
 * @param func Evaluator function.
 * @return LazyEval<F> Lazy evaluator.
 */
template <typename F> constexpr auto lazy_eval(F &&func) -> decltype(auto) {
    return LazyEval<F>(std::forward<F>(func));
}

/**
 * @brief Shared lazy result type.
 *
 * @tparam T Result type.
 */
template <typename T> using LazyResultRc = std::shared_ptr<LazyResult<T>>;

/**
 * @brief Construct a new shared lazy evaluator.
 *
 * @tparam F Evaluator function type.
 * @param func Evaluator function.
 * @return std::shared_ptr<LazyEval<F>> Shared lazy evaluator.
 */
template <typename F> constexpr auto lazy_eval_rc(F &&func) -> decltype(auto) {
    return std::make_shared<LazyEval<F>>(std::forward<F>(func));
}

/**
 * @brief Concept to check if T is derived from LazyResult<R>.
 *
 * @tparam T Target type.
 * @tparam R Return type.
 */
template <typename T, typename R>
concept ILazyResult = std::derived_from<T, LazyResult<R>>;

} // namespace fp
