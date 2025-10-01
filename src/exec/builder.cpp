#include "nexus/exec/builder.hpp"
#include "nexus/exec/policy.hpp"
#include "nexus/exec/pool.hpp"

#include <cstddef>
#include <thread>

namespace nexus::exec {

constexpr std::size_t FALLBACK_MAX_WORKERS = 16;
constexpr std::size_t FALLBACK_MIN_WORKERS = 1;
constexpr std::size_t FALLBACK_INIT_WORKERS = 8;

constexpr std::size_t IO_BOUND_MAX_WORKERS = 200;
constexpr std::size_t IO_BOUND_INIT_WORKERS = 30;

auto blank_builder() -> ThreadPool::Builder {
    return ThreadPool::Builder()
        .policy(TaskPolicy::FIFO)
        .max_workers(FALLBACK_MAX_WORKERS)
        .min_workers(FALLBACK_MIN_WORKERS)
        .init_workers(FALLBACK_INIT_WORKERS)
        .remove_cancelled(false);
}

auto default_builder() -> ThreadPool::Builder {
    std::size_t ncons = std::thread::hardware_concurrency();
    if (ncons == 0) {
        ncons = FALLBACK_MAX_WORKERS;
    }

    return blank_builder().max_workers(ncons).init_workers(ncons / 2);
}

auto cpu_bound_builder() -> ThreadPool::Builder {
    std::size_t ncons = std::thread::hardware_concurrency();
    if (ncons == 0) {
        ncons = FALLBACK_MAX_WORKERS;
    }

    return blank_builder().max_workers((ncons / 2) + 1).init_workers(ncons / 2);
}

auto io_bound_builder() -> ThreadPool::Builder {
    return blank_builder()
        .max_workers(IO_BOUND_MAX_WORKERS)
        .init_workers(IO_BOUND_INIT_WORKERS);
}

auto time_bound_builder() -> ThreadPool::Builder {
    std::size_t ncons = std::thread::hardware_concurrency();
    if (ncons == 0) {
        ncons = FALLBACK_MAX_WORKERS;
    }

    return blank_builder().max_workers(ncons / 2).init_workers(ncons / 2);
}

} // namespace nexus::exec
