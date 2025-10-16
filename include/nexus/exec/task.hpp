#pragma once

#include "nexus/common.hpp"
#include "nexus/private/exec/task.hpp"

#include <any>
#include <compare>
#include <cstdint>
#include <functional>
#include <future>
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
    /**
     * @brief Return type of task.
     *
     */
    using Result = std::decay_t<R>;

    /**
     * @brief Task entry type.
     *
     */
    using DynFunction = std::function<void(std::promise<Result> &)>;

    constexpr static std::int8_t DEFAULT_PRIO = 0;

  private:
    DynFunction _func;
    std::promise<Result> _res;
    std::int8_t _prio{DEFAULT_PRIO};

  public:
    /**
     * @brief Construct a Task.
     *
     * @tparam F Task function type.
     * @tparam Args Task arguments type.
     * @param func Function.
     * @param args Arguments.
     *
     * @note All reference type will be decayed.
     */
    template <typename F, typename... Args>
    explicit Task(F &&func, Args &&...args)
        : _func(_wrap_entry<F, Args...>(std::forward<F>(func),
                                        std::forward<Args>(args)...)) {}

    ~Task() = default;

    Task(const Task &other) = delete;
    auto operator=(const Task &other) -> Task & = delete;

    Task(Task &&other) noexcept = default;
    auto operator=(Task &&other) -> Task & = default;

    /**
     * @brief Sort by priority.
     *
     */
    auto operator<=>(const Task &other) const -> std::weak_ordering {
        return _prio <=> other._prio;
    }

    /**
     * @brief Task function call wrapper.
     *
     */
    NEXUS_INLINE auto operator()() -> void {
        // Pass promise here to avoid invalid `this` pointer caused by
        // `std::move`.
        _func(_res);
    }

    /**
     * @brief Get task future.
     *
     * @return std::future<Result> Task result future.
     */
    NEXUS_INLINE auto get_future() { return _res.get_future(); }

    /**
     * @brief Get task priority.
     *
     * @return int8_t Task priority.
     */
    NEXUS_INLINE constexpr auto prio() -> int8_t { return _prio; }

    /**
     * @brief Set task priority.
     *
     * @param prio New task priority.
     */
    NEXUS_INLINE constexpr auto prio(int8_t prio) -> void { _prio = prio; }

  private:
    /**
     * @brief Wrap function and arguments into entry function, which has
     * signature of `void(std::promise<Result>&)`.
     *
     * @tparam F Function type.
     * @tparam Args Arguments type.
     * @param func Function.
     * @param args Arguments.
     *
     * @note All reference type will be decayed.
     */
    template <typename F, typename... Args>
    NEXUS_INLINE constexpr auto _wrap_entry(F &&func, Args &&...args)
        -> decltype(auto) {
        using Helper = detail::TaskHelper<F, Result, Args...>;
        return typename Helper::Binder(std::forward<F>(func),
                                       std::forward<Args>(args)...);
    }
};

} // namespace nexus::exec
