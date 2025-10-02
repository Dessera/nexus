#include "nexus/exec/task.hpp"
#include "nexus/private/exec/queue.hpp"

#include <deque>
#include <memory>
#include <utility>

namespace nexus::exec::detail {

/**
 * @brief Task queue implementation with FIFO policy.
 *
 */
class FIFO_TaskQueueInner : public TaskQueueInner {
  private:
    std::deque<Task<>> _queue;

  public:
    FIFO_TaskQueueInner() = default;

    auto push(Task<> &&task) -> void override {
        _queue.push_back(std::move(task));
    }

    auto pop() -> Task<> override {
        auto task = std::move(_queue.front());
        _queue.pop_front();
        return task;
    }

    auto size() -> std::size_t override { return _queue.size(); };
};

auto _make_fifo_queue() -> std::unique_ptr<TaskQueueInner> {
    return std::make_unique<FIFO_TaskQueueInner>();
}

} // namespace nexus::exec::detail
