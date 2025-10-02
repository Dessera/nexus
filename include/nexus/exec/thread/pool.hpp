#pragma once

#include "nexus/common.hpp"
#include "nexus/exec/policy.hpp"
#include "nexus/exec/queue.hpp"
#include "nexus/exec/thread/worker.hpp"

#include <cstddef>
#include <deque>
#include <future>
#include <list>
#include <memory>
#include <mutex>
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

    /**
     * @brief Thread pool configuration.
     *
     */
    struct Config {
        TaskPolicy  policy;       /**< Queue policy. */
        std::size_t max_workers;  /**< Max workers (threads). */
        std::size_t min_workers;  /**< Min workers (threads). */
        std::size_t init_workers; /**< Init workers (threads). */
        bool remove_cancelled; /**< Remove cancelled workers in next resize. */
    };

    class Builder {
      private:
        Config _cfg;

      public:
        NEXUS_INLINE auto policy(TaskPolicy policy) -> Builder & {
            _cfg.policy = policy;
            return *this;
        }

        NEXUS_INLINE auto max_workers(std::size_t cnt) -> Builder & {
            _cfg.max_workers = cnt;
            return *this;
        }

        NEXUS_INLINE auto min_workers(std::size_t cnt) -> Builder & {
            _cfg.min_workers = cnt;
            return *this;
        }

        NEXUS_INLINE auto init_workers(std::size_t cnt) -> Builder & {
            _cfg.init_workers = cnt;
            return *this;
        }

        NEXUS_INLINE auto remove_cancelled(bool flag) -> Builder & {
            _cfg.remove_cancelled = flag;
            return *this;
        }

        [[nodiscard]] NEXUS_INLINE auto provide() const -> const Config & {
            return _cfg;
        }

        NEXUS_INLINE auto build() -> ThreadPool { return {_cfg}; }
    };

  private:
    Config _cfg;

    QueuePtr                 _queue;
    std::deque<ThreadWorker> _workers;
    std::list<ThreadWorker>  _cancelled_workers;

    std::mutex _lock;

  public:
    ThreadPool(const Config &cfg);

    ~ThreadPool();

    ThreadPool(const ThreadPool &other) = delete;
    auto operator=(const ThreadPool &other) -> ThreadPool & = delete;

    ThreadPool(ThreadPool &&other) noexcept = delete;
    auto operator=(ThreadPool &&other) -> ThreadPool & = delete;

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
    NEXUS_INLINE auto release() -> void {
        auto guard = std::lock_guard(_lock);

        _cancel_workers(_workers.size());
    }

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

    /**
     * @brief Get thread pool status.
     *
     * @return Report Thread pool status.
     */
    [[nodiscard]] auto report() -> Report;

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

    /**
     * @brief Clean workers that already cancelled.
     *
     * @return std::size_t Workers that are actually cleaned.
     */
    auto _clean_cancelled_workers() -> std::size_t;
};

} // namespace nexus::exec
