#pragma once

#include "nexus/exec/policy.hpp"
#include "nexus/exec/task.hpp"

#include <deque>
#include <utility>

namespace nexus::exec::detail {

template <typename R, TaskPolicy P> class TaskQueueInner {};

template <typename R> class TaskQueueInner<R, TaskPolicy::FIFO> {
  private:
    std::deque<Task<R>> _queue;

  public:
    TaskQueueInner() = default;

    auto push(Task<R> &&task) -> void { _queue.push_back(std::move(task)); }

    auto pop() -> Task<R> {
        auto task = std::move(_queue.front());
        _queue.pop_front();
        return task;
    }

    auto size() { return _queue.size(); }
};

template <typename R> class TaskQueueInner<R, TaskPolicy::LIFO> {
  private:
    std::deque<Task<R>> _queue;

  public:
    TaskQueueInner() = default;

    auto push(Task<R> &&task) -> void { _queue.push_back(std::move(task)); }

    auto pop() -> Task<R> {
        auto task = std::move(_queue.back());
        _queue.pop_back();
        return task;
    }

    auto size() { return _queue.size(); }
};

} // namespace nexus::exec::detail
