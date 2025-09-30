#include <cassert>
#include <future>
#include <nexus/exec/task.hpp>
#include <stdexcept>

auto main() -> int {
    using nexus::exec::Task;

    auto success_task =
        Task<double>([](int arg1, double arg2) { return arg1 + arg2; }, 1, 2);
    success_task();

    auto success_task_future = success_task.get_future();
    auto success_task_result = success_task_future.get();
    assert(success_task_result == 3);

    auto success_task_void = Task<void>([]() {});
    success_task_void();

    auto success_task_void_future = success_task_void.get_future();
    success_task_void_future.get();

    auto failed_task =
        Task<void>([]() { throw std::runtime_error("exception"); });
    failed_task(); // Should not throw here.

    auto failed_task_future = failed_task.get_future();
    try {
        failed_task_future.get();
        assert(false);
    } catch (const std::runtime_error &err) {
        (void)err;
    } catch (...) {
        assert(false);
    }
}
