#pragma once

#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>

namespace fp {

/**
 * @brief Lazy value evaluator.
 *
 * @tparam F Evaluator function type.
 */
template <typename F>
    requires(std::is_invocable_v<F>)
class LazyEval {
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
    using Result = std::decay_t<std::invoke_result_t<Evaluator>>;

  private:
    Evaluator _func;
    mutable Result _result;
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

    /**
     * @brief Get const reference to result.
     *
     * @return const Result& Result reference.
     */
    [[nodiscard]] constexpr auto get_cref() const -> const Result & {
        std::call_once(_flag, [this]() { _result = _func(); });
        return _result;
    }

    /**
     * @brief Get copied value from result.
     *
     * @return Result Result value.
     */
    [[nodiscard]] constexpr auto get() const -> Result { return get_cref(); }

    /**
     * @brief Alias for `get_cref`, get const reference to result.
     *
     * @return const Result& Result reference.
     */
    [[nodiscard]] constexpr auto operator*() const -> decltype(auto) {
        return get_cref();
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
 * @brief Construct a new shared lazy evaluator.
 *
 * @tparam F Evaluator function type.
 * @param func Evaluator function.
 * @return std::shared_ptr<LazyEval<F>> Shared lazy evaluator.
 */
template <typename F> constexpr auto lazy_eval_rc(F &&func) -> decltype(auto) {
    return std::make_shared<LazyEval<F>>(std::forward<F>(func));
}

} // namespace fp
