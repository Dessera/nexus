#pragma once

#include "nexus/common.hpp"
#include "nexus/exec/pool.hpp"

namespace nexus::exec {

/**
 * @brief Get blank thread pool builder.
 *
 * @return ThreadPool::Builder Thread pool builder.
 */
NEXUS_EXPORT auto blank_builder() -> ThreadPool::Builder;

/**
 * @brief Get default thread pool builder.
 *
 * @return ThreadPool::Builder Thread pool builder.
 */
NEXUS_EXPORT auto default_builder() -> ThreadPool::Builder;

/**
 * @brief Get thread pool builder for cpu bound task.
 *
 * @return ThreadPool::Builder Thread pool builder.
 */
NEXUS_EXPORT auto cpu_bound_builder() -> ThreadPool::Builder;

/**
 * @brief Get thread pool builder for io bound task.
 *
 * @return ThreadPool::Builder Thread pool builder.
 */
NEXUS_EXPORT auto io_bound_builder() -> ThreadPool::Builder;

/**
 * @brief Get thread pool builder for timer / logger.
 *
 * @return ThreadPool::Builder Thread pool builder.
 */
NEXUS_EXPORT auto time_bound_builder() -> ThreadPool::Builder;

} // namespace nexus::exec
