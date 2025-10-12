#include "nexus/exec/thread.hpp"

#include <any>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <format>
#include <future>
#include <iostream>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace {

enum class BuilderType : uint8_t { Common, CpuBound, IOBound, Time };

enum class TaskType : uint8_t { Sleep, TinyLoop, MidLoop, LargeLoop };

struct TestArgs {
    BuilderType builder;
    TaskType    task_type;
    std::size_t task_cnt;
    std::size_t thread_cnt;
};

auto null_tester() -> std::size_t { return 0ULL; }

auto sleep_tester() -> std::size_t {
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(2ms);
    return 0ULL;
}

auto loop_tester_1d4c0() {
    constexpr static std::size_t MAX_LOOPS = 120000;
    volatile std::size_t         num = 0;
    for (std::size_t i = 0; i < MAX_LOOPS; ++i) {
        num = 0;
    }
    return num;
}

auto loop_tester_124f80() {
    constexpr static std::size_t MAX_LOOPS = 1200000;
    volatile std::size_t         num = 0;
    for (std::size_t i = 0; i < MAX_LOOPS; ++i) {
        num = 0;
    }
    return num;
}

auto loop_tester_b71b00() {
    constexpr static std::size_t MAX_LOOPS = 12000000;
    volatile std::size_t         num = 0;
    for (std::size_t i = 0; i < MAX_LOOPS; ++i) {
        num = 0;
    }
    return num;
}

auto parse_builder_type(std::string_view str) -> std::optional<BuilderType> {
    if (str == "common") {
        return BuilderType::Common;
    }

    if (str == "cpu") {
        return BuilderType::CpuBound;
    }

    if (str == "io") {
        return BuilderType::IOBound;
    }

    if (str == "time") {
        return BuilderType::Time;
    }

    return {};
}

auto parse_task_type(std::string_view str) -> std::optional<TaskType> {
    if (str == "sleep") {
        return TaskType::Sleep;
    }

    if (str == "tinyloop") {
        return TaskType::TinyLoop;
    }

    if (str == "midloop") {
        return TaskType::MidLoop;
    }

    if (str == "largeloop") {
        return TaskType::LargeLoop;
    }

    return {};
}

auto parse_args(const std::span<char *> &args) -> std::optional<TestArgs> {
    if (args.size() < 5) { // NOLINT
        std::cerr << std::format(
            "Usage: {} <builder> <task_type> <task_cnt> <thread_cnt>\n",
            args[0]);
        return {};
    }

    auto builder_type_result = parse_builder_type(args[1]);
    if (!builder_type_result.has_value()) {
        std::cerr << std::format("Error: {} is not a valid builder\n", args[1]);
        return {};
    }

    auto task_type_result = parse_task_type(args[2]);
    if (!task_type_result.has_value()) {
        std::cerr << std::format("Error: {} is not a valid task type\n",
                                 args[2]);
        return {};
    }

    std::size_t task_cnt = 0;
    try {
        task_cnt = std::stoull(std::string(args[3]));
    } catch (std::exception &err) {
        std::cerr << std::format("Error: {}\n", err.what());
        return {};
    }

    std::size_t thread_cnt = 0;
    try {
        thread_cnt = std::stoull(std::string(args[4]));
    } catch (std::exception &err) {
        std::cerr << std::format("Error: {}\n", err.what());
        return {};
    }

    return TestArgs{.builder = builder_type_result.value(),
                    .task_type = task_type_result.value(),
                    .task_cnt = task_cnt,
                    .thread_cnt = thread_cnt};
}

auto get_builder(BuilderType type) {
    namespace builder = nexus::exec::thread_builder;

    switch (type) {
    case BuilderType::Common:   return builder::common();
    case BuilderType::CpuBound: return builder::cpu_bound();
    case BuilderType::IOBound:  return builder::io_bound();
    case BuilderType::Time:     return builder::time_bound();
    default:                    return builder::blank();
    }
}

auto get_tester(TaskType type) {
    switch (type) {
    case TaskType::Sleep:     return sleep_tester;
    case TaskType::TinyLoop:  return loop_tester_1d4c0;
    case TaskType::MidLoop:   return loop_tester_124f80;
    case TaskType::LargeLoop: return loop_tester_b71b00;
    default:                  return null_tester;
    }
}

} // namespace

auto main(int argc, char **argv) -> int {
    // Parse args
    auto args_str = std::span(argv, argc);
    auto args_result = parse_args(args_str);
    if (!args_result.has_value()) {
        return 1;
    }
    auto args = args_result.value();

    // Get testers
    auto tester = get_tester(args.task_type);

    // Build pool
    auto builder = get_builder(args.builder);
    auto pool = builder.build();
    pool.resize_workers(args.thread_cnt);

    // Time start
    auto start = std::chrono::high_resolution_clock::now();

    auto futs = std::vector<std::future<std::any>>(args.task_cnt);
    for (std::size_t i = 0; i < args.task_cnt; ++i) {
        futs[i] = pool.emplace(tester);
    }

    auto insert_end = std::chrono::high_resolution_clock::now();

    for (std::size_t i = 0; i < args.task_cnt; ++i) {
        futs[i].get();
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> insert_time = insert_end - start;
    std::chrono::duration<double> total_time = end - start;

    // Statistics
    std::cout << "Statistics:\n";
    std::cout << "  Builder: " << args_str[1] << '\n';
    std::cout << "  Task   : " << args_str[2] << '\n';
    std::cout << "  Count  : " << args_str[3] << '\n';
    std::cout << "  Threads: " << args_str[4] << '\n';
    std::cout << "  Insert : " << insert_time.count() << " s\n";
    std::cout << "  Total  : " << total_time.count() << " s\n";
    std::cout << "  Tps    : " << (double)args.task_cnt / total_time.count()
              << " t/s\n";
    std::cout << "  Average: " << total_time.count() / (double)args.task_cnt
              << " s\n";
}
