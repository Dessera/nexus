#pragma once

#include "nexus/exec/task.hpp"

#include <cstddef>

namespace nexus::exec::detail {

/**
 * @brief Task queue implementation.
 *
 */
class TaskQueueInner {
  public:
    TaskQueueInner() = default;
    virtual ~TaskQueueInner() = default;

    TaskQueueInner(const TaskQueueInner &other) = delete;
    auto operator=(const TaskQueueInner &other) -> TaskQueueInner & = delete;

    TaskQueueInner(TaskQueueInner &&other) = delete;
    auto operator=(TaskQueueInner &&other) -> TaskQueueInner & = delete;

    /**
     * @brief Push task into queue.
     *
     * @param task Task object.
     */
    virtual auto push(Task<> &&task) -> void = 0;

    /**
     * @brief Pop task from queue.
     *
     * @return Task<R> Task object.
     */
    virtual auto pop() -> Task<> = 0;

    /**
     * @brief Get queue size.
     *
     * @return std::size_t Queue size.
     */
    virtual auto size() -> std::size_t = 0;
};

/**
 * @brief Create FIFO queue.
 *
 * @return std::unique_ptr<TaskQueueInner> Queue pointer.
 */
auto _make_fifo_queue() -> std::unique_ptr<TaskQueueInner>;

/**
 * @brief Create LIFO queue.
 *
 * @return std::unique_ptr<TaskQueueInner> Queue pointer.
 */
auto _make_lifo_queue() -> std::unique_ptr<TaskQueueInner>;

/**
 * @brief Create PRIO queue.
 *
 * @return std::unique_ptr<TaskQueueInner> Queue pointer.
 */
auto _make_prio_queue() -> std::unique_ptr<TaskQueueInner>;

/**
 * @brief Create RAND queue.
 *
 * @return std::unique_ptr<TaskQueueInner> Queue pointer.
 */
auto _make_rand_queue() -> std::unique_ptr<TaskQueueInner>;

} // namespace nexus::exec::detail
