#pragma once

#include "nexus/common.hpp"
#include "nexus/exec/queue.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

namespace nexus::exec {

/**
 * @brief Worker thread.
 *
 */
class NEXUS_EXPORT Worker {
  public:
    /**
     * @brief Queue pointer type for sharing ownership.
     *
     */
    using QueuePtr = std::shared_ptr<TaskQueue>;

    /**
     * @brief Jthread pointer type for lazy initialization.
     *
     */
    using ThreadPtr = std::unique_ptr<std::jthread>;

    /**
     * @brief Worker status.
     *
     */
    enum class Status : uint8_t {
        Create,     // Worker is created.
        Running,    // Worker is running.
        CancelWait, // Worker is running, but will be cancelled soon.
        Cancel,     // Worker is cancelled.
    };

    /**
     * @brief Inner class for move worker safely.
     *
     */
    struct Inner {
        std::atomic<Status>     status{Status::Create};
        std::mutex              lock;
        std::condition_variable cancel_notify;
    };

    /**
     * @brief Inner pointer for sharing ownership.
     *
     */
    using InnerPtr = std::shared_ptr<Inner>;

  private:
    QueuePtr  _queue;
    ThreadPtr _worker{nullptr};
    InnerPtr  _inner{std::make_shared<Inner>()};

  public:
    Worker(QueuePtr &&queue) : _queue(std::move(queue)) {}

    Worker(const QueuePtr &queue) : _queue(queue) {}

    ~Worker() = default;

    Worker(const Worker &other) = delete;
    auto operator=(const Worker &other) -> Worker & = delete;

    Worker(Worker &&other) noexcept = default;
    auto operator=(Worker &&other) -> Worker & = default;

    /**
     * @brief Run a worker, if worker is running (Running or CancelWait),
     * the operation is assumed to be failed.
     *
     * @return true Successed to run worker.
     * @return false Failed to run worker.
     */
    auto run() -> bool;

    /**
     * @brief Cancel a worker, if worker is cancelled (Create or Cancel), the
     * operation is assumed to be failed.
     *
     * @return true Successed to mark worker as CancelWait.
     * @return false Failed to cancel worker.
     */
    auto cancel() -> bool;

    /**
     * @brief Cancel the `Cancel` operation,if worker is running, the operation
     * is assumed to be failed.
     *
     * @return true Successed to uncancel worker.
     * @return false Failed to uncancel worker.
     */
    auto uncancel() -> bool;

    /**
     * @brief Wait for worker cancelled.
     *
     */
    auto wait_for_cancel() -> void;

    /**
     * @brief Get worker status.
     *
     * @return Status Worker status.
     */
    [[nodiscard]] NEXUS_INLINE auto status() const -> Status {
        return _inner->status.load();
    }

    /**
     * @brief Check if worker is in cancel wait.
     *
     * @return true Worker is in cancel wait.
     * @return false Worker is not in cancel wait.
     */
    [[nodiscard]] NEXUS_INLINE auto is_cancel_wait() const -> bool {
        return status() == Status::CancelWait;
    }

    /**
     * @brief Check if worker is cancelled.
     *
     * @return true Worker is cancelled.
     * @return false Worker is not cancelled.
     */
    [[nodiscard]] NEXUS_INLINE auto is_cancelled() const -> bool {
        return status() == Status::Cancel;
    }

    /**
     * @brief Check if worker is running.
     *
     * @return true Worker is running.
     * @return false Worker is not running.
     */
    [[nodiscard]] NEXUS_INLINE auto is_running() const -> bool {
        return status() == Status::Running;
    }

    /**
     * @brief Check if worker is created.
     *
     * @return true Worker is created.
     * @return false Worker is not created.
     */
    [[nodiscard]] NEXUS_INLINE auto is_created() const -> bool {
        return status() == Status::Create;
    }

    /**
     * @brief Wait for worker cancelled.
     *
     * @param timeout Wait timeout.
     * @return true The worker was cancelled successfully.
     * @return false Timeout.
     */
    template <typename Rep, typename Period>
    auto wait_for_cancel(const std::chrono::duration<Rep, Period> &timeout)
        -> bool {
        auto guard = std::unique_lock(_inner->lock);
        auto status = _inner->cancel_notify.wait_for(
            guard, timeout, [this]() { return is_cancelled(); });

        return status != std::cv_status::timeout;
    }

  private:
    /**
     * @brief Worker loop, take task and execute it.
     *
     */
    static auto _worker_loop(const QueuePtr &queue, InnerPtr &inner) -> void;
};

} // namespace nexus::exec
