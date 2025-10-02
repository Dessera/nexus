#include "nexus/exec/task.hpp"
#include "nexus/private/exec/queue.hpp"

#include <deque>
#include <utility>

namespace nexus::exec::detail {

/**
 * @brief Task queue implementation with LIFO policy.
 *
 */
class LIFO_TaskQueueInner : public TaskQueueInner {
  private:
    std::deque<Task<>> _queue;

  public:
    LIFO_TaskQueueInner() = default;

    auto push(Task<> &&task) -> void override {
        _queue.push_back(std::move(task));
    }

    auto pop() -> Task<> override {
        auto task = std::move(_queue.back());
        _queue.pop_back();
        return task;
    }

    auto size() -> std::size_t override { return _queue.size(); };
};

auto _make_lifo_queue() -> std::unique_ptr<TaskQueueInner> {
    return std::make_unique<LIFO_TaskQueueInner>();
}

} // namespace nexus::exec::detail
