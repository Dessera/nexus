#pragma once

#include "nexus/common.hpp"
#include "nexus/exec/thread/pool.hpp"

namespace nexus::exec::thread_builder {

/**
 * @brief Get blank thread pool builder.
 *
 * @return ThreadPool::Builder Thread pool builder.
 */
NEXUS_EXPORT auto blank() -> ThreadPool::Builder;

/**
 * @brief Get common thread pool builder.
 *
 * @return ThreadPool::Builder Thread pool builder.
 */
NEXUS_EXPORT auto common() -> ThreadPool::Builder;

/**
 * @brief Get thread pool builder for cpu bound task.
 *
 * @return ThreadPool::Builder Thread pool builder.
 */
NEXUS_EXPORT auto cpu_bound() -> ThreadPool::Builder;

/**
 * @brief Get thread pool builder for io bound task.
 *
 * @return ThreadPool::Builder Thread pool builder.
 */
NEXUS_EXPORT auto io_bound() -> ThreadPool::Builder;

/**
 * @brief Get thread pool builder for timer / logger.
 *
 * @return ThreadPool::Builder Thread pool builder.
 */
NEXUS_EXPORT auto time_bound() -> ThreadPool::Builder;

} // namespace nexus::exec::thread_builder
