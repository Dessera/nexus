#pragma once

#include "nexus/common.hpp"
#include "nexus/exec/policy.hpp"
#include "nexus/exec/task.hpp"
#include "nexus/private/exec/queue.hpp"

#include <any>
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <utility>

namespace nexus::exec {

/**
 * @brief Thread safe task queue.
 *
 */
class NEXUS_EXPORT TaskQueue {
  public:
    /**
     * @brief Return type of task in queue.
     *
     */
    using Result = std::any;

    /**
     * @brief Task type in queue.
     *
     */
    using TaskType = Task<Result>;

    /**
     * @brief Task implementation.
     *
     */
    using InnerPtr = std::unique_ptr<detail::TaskQueueInner>;

  private:
    static const std::unordered_map<TaskPolicy, InnerPtr (*)()>
        POLICY_CREATOR; /**< TaskPolicy to specific queue. */

    InnerPtr _inner;

    std::mutex _lock;
    std::condition_variable _cond;
    std::atomic_size_t _size{0};

  public:
    TaskQueue(TaskPolicy policy) : _inner(POLICY_CREATOR.at(policy)()) {}
    ~TaskQueue() = default;

    TaskQueue(const TaskQueue &other) = delete;
    auto operator=(const TaskQueue &other) -> TaskQueue & = delete;

    TaskQueue(TaskQueue &&other) = delete;
    auto operator=(TaskQueue &&other) -> TaskQueue & = delete;

    /**
     * @brief Add a task to the queue.
     *
     * @param task Task object.
     */
    auto push(TaskType &&task) -> void;

    /**
     * @brief Pop one task (wait until queue is ready).
     *
     * @return TaskType Task object.
     */
    auto pop() -> TaskType;

    /**
     * @brief Get task count.
     *
     * @return std::size_t Task count.
     */
    NEXUS_INLINE auto size() -> std::size_t { return _size.load(); }

    /**
     * @brief Get if queue is empty.
     *
     * @return true Queue is empty.
     * @return false Queue is not empty.
     */
    NEXUS_INLINE auto empty() -> bool { return _size.load() == 0; }

    /**
     * @brief Wake up all worker listening on the queue.
     *
     */
    NEXUS_INLINE auto wakeup_all() -> void { _cond.notify_all(); }

    /**
     * @brief Add a task to the queue.
     *
     * @tparam Args Task construct arguments type.
     * @param args Task construct arguments.
     */
    template <typename... Args>
    NEXUS_INLINE auto emplace(Args &&...args) -> void {
        push(TaskType(std::forward<Args>(args)...));
    }

    /**
     * @brief  Pop one task (wait until queue is ready or timeout).
     *
     * @param timeout Wait timeout.
     * @return std::optional<TaskType> Task object.
     */
    template <typename Rep, typename Period>
    auto pop_for(const std::chrono::duration<Rep, Period> &timeout)
        -> std::optional<TaskType> {
        auto guard = std::unique_lock(_lock);
        auto status =
            _cond.wait_for(guard, timeout, [this]() { return !this->empty(); });

        // Timeout
        if (!status) {
            return {};
        }

        return _pop_impl();
    }

    /**
     * @brief  Pop one task (wait until queue is ready or pred).
     *
     * @param pred User pred function.
     * @return std::optional<TaskType> Task object.
     */
    template <typename F> auto pop(F &&pred) -> std::optional<TaskType> {
        auto guard = std::unique_lock(_lock);

        auto is_user_pred = false;
        _cond.wait(guard,
                   [this, &is_user_pred, pred = std::forward<F>(pred)]() {
                       if (pred()) {
                           is_user_pred = true;
                           return true;
                       }

                       return !this->empty();
                   });

        // User pred
        if (is_user_pred) {
            return {};
        }

        return _pop_impl();
    }

  private:
    /**
     * @brief Wrapper of pop, which is used to update empty flag.
     *
     * @return TaskType Task object.
     */
    auto _pop_impl() -> TaskType;
};

} // namespace nexus::exec
