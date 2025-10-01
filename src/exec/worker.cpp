#include "nexus/exec/worker.hpp"

namespace nexus::exec {

auto Worker::run() -> bool {
    auto guard = std::lock_guard(_inner->lock);

    if (is_running() || is_cancel_wait()) {
        return false;
    }

    _worker = std::make_unique<std::jthread>(
        [queue = this->_queue, inner = this->_inner]() mutable {
            _worker_loop(queue, inner);
        });
    _inner->status.store(Status::Running);

    return true;
}

auto Worker::cancel() -> bool {
    auto guard = std::lock_guard(_inner->lock);

    if (is_cancelled() || is_created()) {
        return false;
    }

    _inner->status.store(Status::CancelWait);

    return true;
}

auto Worker::uncancel() -> bool {
    auto guard = std::unique_lock(_inner->lock);

    // Running => Don't need to uncancel.
    if (is_running()) {
        return false;
    }

    // CancelWait => Resume to Running.
    if (is_cancel_wait()) {
        _inner->status.store(Status::Running);
        return true;
    }

    // Other status => Rerun.
    guard.unlock();
    return run();
}

auto Worker::wait_for_cancel() -> void {
    auto guard = std::unique_lock(_inner->lock);
    _inner->cancel_notify.wait(guard, [this]() { return is_cancelled(); });
}

auto Worker::_worker_loop(const QueuePtr &queue, InnerPtr &inner) -> void {
    while (true) {
        auto task = queue->pop(
            [&inner]() { return inner->status.load() == Status::CancelWait; });

        if (task.has_value()) {
            task.value()();
        }

        auto guard = std::unique_lock(inner->lock);
        if (inner->status == Status::CancelWait) {
            inner->status.store(Status::Cancel);

            guard.unlock();
            inner->cancel_notify.notify_all();

            break;
        }
    }
}

} // namespace nexus::exec
