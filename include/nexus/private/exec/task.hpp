#pragma once

#include <future>
#include <tuple>
#include <type_traits>
#include <utility>

namespace nexus::exec::detail {

/**
 * @brief Task function binder.
 *
 * @tparam F Function type.
 * @tparam R Return type.
 * @tparam Args Function arguments type.
 */
template <typename F, typename R, typename... Args>
    requires(std::is_invocable_v<F, Args...> &&
             std::is_convertible_v<std::invoke_result_t<F, Args...>, R>)
class TaskBinder {
  public:
    /**
     * @brief Actual return type.
     *
     */
    using WrappedResult = R;

    /**
     * @brief Inner return type.
     *
     */
    using Result = std::invoke_result_t<F, Args...>;

    /**
     * @brief Arguments tuple type.
     *
     */
    using ArgsTuple = std::tuple<Args...>;

  private:
    F _func;
    ArgsTuple _args;

  public:
    explicit constexpr TaskBinder(const F &func, Args &&...args)
        : _func(func), _args(std::forward<Args>(args)...) {}

    explicit constexpr TaskBinder(F &&func, Args &&...args)
        : _func(std::move(func)), _args(std::forward<Args>(args)...) {}

    auto operator()(std::promise<WrappedResult> &res) -> void {
        static_cast<const TaskBinder *>(this)->operator()(res);
    }

    /**
     * @brief Wrapped function body.
     *
     * @param res Promise to pass return value.
     */
    auto operator()(std::promise<WrappedResult> &res) const -> void {
        try {
            if constexpr (std::is_same_v<WrappedResult, void>) {
                std::apply(_func, _args);
                res.set_value();
            } else {
                res.set_value(
                    static_cast<WrappedResult>(std::apply(_func, _args)));
            }
        } catch (...) {
            res.set_exception(std::current_exception());
        }
    }
};

/**
 * @brief Task helper to wrap function.
 *
 * @tparam F Function type.
 * @tparam R Return type.
 * @tparam Args Function arguments type.
 */
template <typename F, typename R, typename... Args> struct TaskHelper {

    /**
     * @brief Dereferenced function type.
     *
     */
    using Function = std::decay_t<F>;

    /**
     * @brief Dereferenced return type.
     *
     */
    using Result = std::decay_t<R>;

    /**
     * @brief Task binder.
     *
     */
    using Binder = TaskBinder<Function, Result, std::decay_t<Args>...>;
};

} // namespace nexus::exec::detail
