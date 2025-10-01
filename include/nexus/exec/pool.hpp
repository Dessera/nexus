#pragma once

#include "nexus/common.hpp"
#include "nexus/exec/policy.hpp"
#include "nexus/exec/queue.hpp"
#include "nexus/exec/worker.hpp"

#include <cstddef>
#include <deque>
#include <future>
#include <memory>
#include <utility>

namespace nexus::exec {

/**
 * @brief Thread pool to execute task.
 *
 */
class NEXUS_EXPORT ThreadPool {
  public:
    /**
     * @brief Return type of task in pool.
     *
     */
    using Result = TaskQueue::Result;

    /**
     * @brief Task type in pool.
     *
     */
    using TaskType = TaskQueue::TaskType;

    /**
     * @brief Queue pointer type for sharing ownership.
     *
     */
    using QueuePtr = std::shared_ptr<TaskQueue>;

    /**
     * @brief Thread pool status report.
     *
     */
    struct Report {
        std::size_t running;
        std::size_t cancel_wait;
        std::size_t cancelled;
    };

  private:
    QueuePtr           _queue;
    std::deque<Worker> _workers;
    std::deque<Worker> _cancelled_workers;

    std::size_t _max_workers;
    std::size_t _min_workers;

  public:
    ThreadPool(TaskPolicy policy, std::size_t max_workers,
               std::size_t min_workers); // NOLINT
    ~ThreadPool();

    ThreadPool(const ThreadPool &other) = delete;
    auto operator=(const ThreadPool &other) -> ThreadPool & = delete;

    ThreadPool(ThreadPool &&other) noexcept = default;
    auto operator=(ThreadPool &&other) -> ThreadPool & = default;

    /**
     * @brief Add a task to the queue.
     *
     * @param task Task object.
     * @return std::future<Result> Task future.
     */
    auto push(TaskType &&task) -> std::future<Result>;

    /**
     * @brief Resize the workers queue.
     *
     * @param new_size New workers size.
     *
     * @note min_workers <= new_size <= max_workers
     * @note The method does not gurarantee the immediate effectiveness of the
     * modification.
     */
    auto resize_workers(std::size_t new_size) -> void;

    /**
     * @brief Cancel all workers.
     *
     */
    NEXUS_INLINE auto release() -> void { _cancel_workers(_workers.size()); }

    /**
     * @brief Add a task to the queue.
     *
     * @tparam Args Task construct arguments type.
     * @param args Task construct arguments.
     * @return std::future<Result> Task future.
     */
    template <typename... Args> auto emplace(Args &&...args) -> decltype(auto) {
        return push(TaskType(std::forward<Args>(args)...));
    }

  private:
    /**
     * @brief Reuse cancelled workers.
     *
     * @param need Workers count to be reused.
     * @return std::size_t Workers that are actually reused.
     */
    auto _reuse_workers(std::size_t need) -> std::size_t;

    /**
     * @brief Cancel workers.
     *
     * @param need Workers count to be cancelled.
     * @return std::size_t Workers that are actually cancelled.
     */
    auto _cancel_workers(std::size_t need) -> std::size_t;
};

} // namespace nexus::exec
