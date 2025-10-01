#pragma once

#include "nexus/exec/policy.hpp"
#include "nexus/exec/queue.hpp"
#include "nexus/exec/worker.hpp"

#include <algorithm>
#include <any>
#include <cstddef>
#include <list>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <utility>

namespace nexus::exec {

template <TaskPolicy P> class ThreadPool {
  public:
    using Result = std::any;
    using QueuePtr = std::shared_ptr<TaskQueue<Result, P>>;

  private:
    QueuePtr _queue{std::make_shared<TaskQueue<Result, P>>()};
    std::list<Worker<Result, P>> _workers;
    std::list<Worker<Result, P>> _cancelled_workers;

    std::size_t _max_workers;
    std::size_t _min_workers;

    std::mutex _lock;

  public:
    ThreadPool(std::size_t max_workers, std::size_t min_workers) // NOLINT
        : _max_workers(max_workers), _min_workers(min_workers) {
        if (max_workers < min_workers) {
            throw std::range_error("max_workers is smaller than min_workers");
        }

        resize_workers(min_workers);
    }

    auto resize_workers(std::size_t new_size) -> std::size_t {
        auto guard = std::lock_guard(_lock);

        new_size = std::max(new_size, _min_workers);
        new_size = std::min(new_size, _max_workers);

        auto prev_size = _workers.size();

        // No need to be adjusted.
        if (prev_size == new_size) {
            return 0;
        }

        // Need to add workers:
        //   1. Reuse from _cancelled_workers.
        //   2. Create new Workers.
        if (prev_size < new_size) {
            auto diff = new_size - prev_size;
            diff -= _reuse_workers(diff);
            for (std::size_t i = 0; i < diff; ++i) {
                _workers.push_back(Worker(_queue));
                _workers.back().run();
            }
            return diff;
        }

        // Need to remove workers:
        //   1. Add to _cancelled_workers.
        //   2. Pop workers and mark it as cancelled.
        auto diff = prev_size - new_size;
        for (std::size_t i = 0; i < diff; ++i) {
            _cancelled_workers.push_back(std::move(_workers.front()));
            _workers.pop_front();

            _cancelled_workers.back().cancel();
        }
        return diff;
    }

  private:
    /**
     * @brief Reuse cancelled workers.
     *
     * @param need Workers count to be reused.
     * @return std::size_t Workers that are actually reused.
     */
    auto _reuse_workers(std::size_t need) -> std::size_t {
        std::size_t wake_cnt = 0;
        while (!_cancelled_workers.empty() && wake_cnt < need) {
            _workers.push_back(std::move(_cancelled_workers.front()));
            _cancelled_workers.pop_front();

            _workers.back().uncancel();
            ++wake_cnt;
        }

        return wake_cnt;
    }
};

} // namespace nexus::exec
