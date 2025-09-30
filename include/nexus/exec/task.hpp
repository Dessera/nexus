#pragma once

#include <any>
#include <exception>
#include <functional>
#include <future>
#include <tuple>
#include <type_traits>
#include <utility>

namespace nexus::exec {

/**
 * @brief Task object for delayed calling.
 *
 * @tparam R Return type of task.
 */
template <typename R = std::any> class Task {
  public:
    using Result = std::decay_t<R>;
    using Function = std::function<void(std::promise<Result> &)>;

  private:
    Function _func;
    std::promise<Result> _res;

  public:
    /**
     * @brief Construct a Task.
     *
     * @tparam F Task function type.
     * @tparam Args Task arguments type.
     * @param func Function.
     * @param args Arguments.
     *
     * @note Disallow to pass a reference type.
     */
    template <typename F, typename... Args>
    Task(F &&func, Args &&...args)
        requires(std::is_invocable_v<F, std::decay_t<Args>...>)
        : _func(_wrap_entry<F, Args...>(std::forward<F>(func),
                                        std::forward<Args>(args)...)) {}

    /**
     * @brief Task function call wrapper.
     *
     */
    auto operator()() -> void {
        // Pass promise here to avoid invalid `this` pointer caused by
        // `std::move`.
        _func(_res);
    }

    /**
     * @brief Get task future.
     *
     * @return std::future<Result> Task result future.
     */
    auto get_future() { return _res.get_future(); }

  private:
    /**
     * @brief Wrap function and arguments into entry function, which has
     * signature of `void()`.
     *
     * @tparam F Function type.
     * @tparam Args Arguments type.
     * @param func Function.
     * @param args Arguments.
     *
     * @note Disallow to pass a reference type.
     */
    template <typename F, typename... Args>
    constexpr auto _wrap_entry(std::decay_t<F> &&func, Args &&...args)
        -> decltype(auto)
        requires(std::is_invocable_v<F, std::decay_t<Args>...>)
    {
        using InvokeResult = std::invoke_result_t<F, std::decay_t<Args>...>;

        auto args_tuple =
            std::tuple<std::decay_t<Args>...>(std::forward<Args>(args)...);
        return [func = std::move(func),
                args_tuple = std::move(args_tuple)](std::promise<Result> &res) {
            try {
                if constexpr (std::is_same_v<InvokeResult, void>) {
                    std::apply(func, args_tuple);
                    res.set_value();
                } else {
                    res.set_value(std::apply(func, args_tuple));
                }
            } catch (...) {
                res.set_exception(std::current_exception());
            }
        };
    }

    /**
     * @brief Wrap function and arguments into entry function, which has
     * signature of `void()`.
     *
     * @tparam F Function type.
     * @tparam Args Arguments type.
     * @param func Function.
     * @param args Arguments.
     *
     * @note Disallow to pass a reference type.
     */
    template <typename F, typename... Args>
    constexpr auto _wrap_entry(const std::decay_t<F> &func, Args &&...args)
        -> decltype(auto)
        requires(std::is_invocable_v<F, std::decay_t<Args>...>)
    {
        auto func_copy = func;
        return _wrap_entry(std::move(func_copy), std::forward<Args>(args)...);
    }
};

} // namespace nexus::exec
