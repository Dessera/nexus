#pragma once

#include "nexus/exec/policy.hpp"
#include "nexus/exec/task.hpp"

#include <deque>
#include <utility>

namespace nexus::exec::detail {

/**
 * @brief Task queue implementation.
 *
 * @tparam R Task result type.
 * @tparam P Queue policy.
 */
template <typename R, TaskPolicy P> class TaskQueueInner {};

/**
 * @brief Task queue implementation in FIFO policy.
 *
 * @tparam R Task result type.
 */
template <typename R> class TaskQueueInner<R, TaskPolicy::FIFO> {
  private:
    std::deque<Task<R>> _queue;

  public:
    TaskQueueInner() = default;

    /**
     * @brief Push task into queue.
     *
     * @param task Task object.
     */
    auto push(Task<R> &&task) -> void { _queue.push_back(std::move(task)); }

    /**
     * @brief Pop task from queue.
     *
     * @return Task<R> Task object.
     */
    auto pop() -> Task<R> {
        auto task = std::move(_queue.front());
        _queue.pop_front();
        return task;
    }

    /**
     * @brief Get queue size.
     *
     * @return std::size_t Queue size.
     */
    auto size() { return _queue.size(); }
};

/**
 * @brief Task queue implementation in LIFO policy.
 *
 * @tparam R Task result type.
 */
template <typename R> class TaskQueueInner<R, TaskPolicy::LIFO> {
  private:
    std::deque<Task<R>> _queue;

  public:
    TaskQueueInner() = default;

    /**
     * @brief Push task into queue.
     *
     * @param task Task object.
     */
    auto push(Task<R> &&task) -> void { _queue.push_back(std::move(task)); }

    /**
     * @brief Pop task from queue.
     *
     * @return Task<R> Task object.
     */
    auto pop() -> Task<R> {
        auto task = std::move(_queue.back());
        _queue.pop_back();
        return task;
    }

    /**
     * @brief Get queue size.
     *
     * @return std::size_t Queue size.
     */
    auto size() { return _queue.size(); }
};

} // namespace nexus::exec::detail
