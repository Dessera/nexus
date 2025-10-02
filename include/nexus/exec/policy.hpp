#pragma once

#include <cstdint>

namespace nexus::exec {

/**
 * @brief Task queue policies.
 *
 */
enum class TaskPolicy : uint8_t {
    FIFO,
    LIFO,
    PRIO,
    RAND,
};

} // namespace nexus::exec
