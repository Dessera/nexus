#include "nexus/exec/queue.hpp"
#include "nexus/exec/policy.hpp"
#include <unordered_map>

namespace nexus::exec {

const std::unordered_map<TaskPolicy, TaskQueue::InnerPtr (*)()>
    TaskQueue::POLICY_CREATOR = {{TaskPolicy::FIFO, detail::_make_fifo_queue},
                                 {TaskPolicy::LIFO, detail::_make_lifo_queue}};

auto TaskQueue::push(TaskType &&task) -> void {
    auto guard = std::unique_lock(_lock);

    _inner->push(std::move(task));
    _size.fetch_add(1);

    guard.unlock();
    _cond.notify_one();
}

auto TaskQueue::pop() -> TaskType {
    auto guard = std::unique_lock(_lock);
    _cond.wait(guard, [this]() { return !this->empty(); });

    return _pop_impl();
}

auto TaskQueue::_pop_impl() -> TaskType {
    auto task = _inner->pop();
    _size.fetch_sub(1);

    return task;
}

} // namespace nexus::exec
