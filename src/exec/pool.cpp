#include "nexus/exec/thread/pool.hpp"
#include "nexus/exec/thread/worker.hpp"

#include <algorithm>
#include <cstddef>
#include <mutex>
#include <utility>

namespace nexus::exec {

ThreadPool::ThreadPool(const Config &cfg)
    : _cfg(cfg), _queue(std::make_shared<TaskQueue>(_cfg.policy)) {
    if (_cfg.max_workers < _cfg.min_workers) {
        throw std::range_error("max_workers is smaller than min_workers");
    }

    resize_workers(_cfg.init_workers);
}

ThreadPool::~ThreadPool() { release(); }

auto ThreadPool::push(TaskType &&task) -> std::future<Result> {
    auto fut = task.get_future();
    _queue->push(std::move(task));
    return fut;
}

auto ThreadPool::resize_workers(std::size_t new_size) -> void {
    auto guard = std::lock_guard(_lock);

    new_size = std::max(new_size, _cfg.min_workers);
    new_size = std::min(new_size, _cfg.max_workers);

    auto prev_size = _workers.size();

    // No need to be adjusted.
    if (prev_size == new_size) {
        return;
    }

    // Need to add workers:
    //   1. Reuse from _cancelled_workers.
    //   2. Create new Workers.
    if (prev_size < new_size) {
        auto diff = new_size - prev_size;
        diff -= _reuse_workers(diff);
        for (std::size_t i = 0; i < diff; ++i) {
            _workers.emplace_back(_queue);
            _workers.back().run();
        }
        return;
    }

    // Need to remove workers:
    //   1. Add to _cancelled_workers.
    //   2. Pop workers and mark it as cancelled.
    _cancel_workers(prev_size - new_size);

    if (_cfg.remove_cancelled) {
        _clean_cancelled_workers();
    }
}

auto ThreadPool::report() -> Report {
    auto guard = std::lock_guard(_lock);

    auto res = Report();
    for (const auto &worker : _cancelled_workers) {
        if (worker.is_cancelled()) {
            ++res.cancelled;
        } else if (worker.is_cancel_wait()) {
            ++res.cancel_wait;
        }
    }

    res.running = _workers.size();
    return res;
}

auto ThreadPool::_reuse_workers(std::size_t need) -> std::size_t {
    std::size_t wake_cnt = 0;
    while (!_cancelled_workers.empty() && wake_cnt < need) {
        _workers.push_back(std::move(_cancelled_workers.front()));
        _cancelled_workers.pop_front();

        _workers.back().uncancel();
        ++wake_cnt;
    }

    return wake_cnt;
}

auto ThreadPool::_cancel_workers(std::size_t need) -> std::size_t {
    std::size_t cancel_cnt = 0;
    while (!_workers.empty() && cancel_cnt < need) {
        _cancelled_workers.push_back(std::move(_workers.front()));
        _workers.pop_front();

        _cancelled_workers.back().cancel();
        ++cancel_cnt;
    }

    _queue->wakeup_all();
    return cancel_cnt;
}

auto ThreadPool::_clean_cancelled_workers() -> std::size_t {
    auto cit = _cancelled_workers.begin();
    std::size_t clean_cnt = 0;

    while (cit != _cancelled_workers.end()) {
        if ((*cit).is_cancelled()) {
            cit = _cancelled_workers.erase(cit);
            ++clean_cnt;
        } else {
            ++cit;
        }
    }

    return clean_cnt;
}

} // namespace nexus::exec
