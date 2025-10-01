#pragma once

#include "nexus/exec/policy.hpp"
#include "nexus/exec/queue.hpp"

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
 * @tparam R Task return type.
 * @tparam P Queue policy.
 */
template <typename R, TaskPolicy P> class Worker {
  public:
    /**
     * @brief Queue pointer type for sharing ownership.
     *
     */
    using QueuePtr = std::shared_ptr<TaskQueue<R, P>>;

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
        Status status{Status::Create};
        std::mutex lock;
        std::condition_variable cancel_notify;
    };

    constexpr static const auto Timeout = std::chrono::milliseconds(100);

  private:
    QueuePtr _queue;
    std::unique_ptr<std::thread> _worker{nullptr};

    std::shared_ptr<Inner> _inner{std::make_shared<Inner>()};

  public:
    Worker(QueuePtr &&queue) : _queue(std::move(queue)) {}

    Worker(const QueuePtr &queue) : _queue(queue) {}

    ~Worker() {
        cancel();

        auto *ptr = _worker.get();
        if (ptr != nullptr && ptr->joinable()) {
            ptr->join();
        }
    }

    Worker(const Worker &other) = delete;
    auto operator=(const Worker &other) -> Worker & = delete;

    Worker(Worker &&other) = default;
    auto operator=(Worker &&other) -> Worker & = default;

    /**
     * @brief Run a worker, if worker is running (Running or CancelWait),
     * the operation is assumed to be failed.
     *
     * @return true Successed to run worker.
     * @return false Failed to run worker.
     */
    auto run() -> bool {
        auto guard = std::lock_guard(_inner->lock);

        if (_inner->status == Status::Running ||
            _inner->status == Status::CancelWait) {
            return false;
        }

        _worker = std::make_unique<std::thread>(
            [queue = this->_queue, inner = this->_inner]() {
                _worker_loop(std::move(queue), std::move(inner));
            });
        _inner->status = Status::Running;
        return true;
    }

    /**
     * @brief Cancel a worker, if worker is cancelled (Create or Cancel), the
     * operation is assumed to be failed.
     *
     * @return true Successed to mark worker as CancelWait.
     * @return false Failed to cancel worker.
     */
    auto cancel() -> bool {
        auto guard = std::lock_guard(_inner->lock);
        if (_inner->status == Status::Cancel ||
            _inner->status == Status::Create) {
            return false;
        }

        _inner->status = Status::CancelWait;
        return true;
    }

    /**
     * @brief Cancel the `Cancel` operation,if worker is running, the operation
     * is assumed to be failed.
     *
     * @return true Successed to uncancel worker.
     * @return false Failed to uncancel worker.
     */
    auto uncancel() -> bool {
        auto guard = std::unique_lock(_inner->lock);

        if (_inner->status == Status::Running) {
            return false;
        }

        if (_inner->status == Status::CancelWait) {
            _inner->status = Status::Running;
            return true;
        }

        guard.unlock();
        return run();
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
        auto status = _inner->cancel_notify.wait_for(guard, timeout, [this]() {
            return _inner->status == Status::Cancel;
        });

        return status != std::cv_status::timeout;
    }

    /**
     * @brief Wait for worker cancelled.
     *
     */
    auto wait_for_cancel() -> void {
        auto guard = std::unique_lock(_inner->lock);
        _inner->cancel_notify.wait(
            guard, [this]() { return _inner->status == Status::Cancel; });
    }

  private:
    /**
     * @brief Worker loop, take task and execute it.
     *
     */
    static void _worker_loop(QueuePtr queue, std::shared_ptr<Inner> inner) {
        while (true) {
            auto task = queue->pop_for(Timeout);

            if (task.has_value()) {
                task.value()();
            }

            auto guard = std::unique_lock(inner->lock);
            if (inner->status == Status::CancelWait) {
                inner->status = Status::Cancel;

                guard.unlock();
                inner->cancel_notify.notify_all();

                break;
            }
        }
    }
};

} // namespace nexus::exec
