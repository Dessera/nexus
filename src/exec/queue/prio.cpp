#include "nexus/common.hpp"
#include "nexus/exec/task.hpp"
#include "nexus/private/exec/queue.hpp"

#include <list>
#include <queue>
#include <utility>

namespace nexus::exec::detail {

/**
 * @brief Task queue implementation with priority.
 *
 */
class PRIO_TaskQueueInner : public TaskQueueInner {
  private:
    /**
     * @brief Helper object to index the task.
     *
     */
    struct TaskHandle {
        std::list<Task<>>::iterator it; // NOLINT

        NEXUS_INLINE auto operator<=>(const TaskHandle &other) const {
            return (*it) <=> (*other.it);
        };
    };

    /**
     * @brief Method `top` of priority_queue returns a `const_reference`, which
     * does not support move.
     * So PRIO_TaskQueueInner uses list to store tasks and uses priority_queue
     * only to index the list.
     *
     */
    std::list<Task<>>               _tasks;
    std::priority_queue<TaskHandle> _queue;

  public:
    PRIO_TaskQueueInner() = default;

    auto push(Task<> &&task) -> void override {
        _tasks.push_back(std::move(task));

        auto task_it = _tasks.end();
        --task_it;

        _queue.push({.it = task_it});
    }

    auto pop() -> Task<> override {
        auto task_handle = _queue.top();
        _queue.pop();

        auto task = std::move(*task_handle.it);
        _tasks.erase(task_handle.it);

        return task;
    }

    auto size() -> std::size_t override { return _queue.size(); };
};

auto _make_prio_queue() -> std::unique_ptr<TaskQueueInner> {
    return std::make_unique<PRIO_TaskQueueInner>();
}

} // namespace nexus::exec::detail
