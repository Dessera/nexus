#pragma once

#include <cstdint>

namespace nexus::exec {

/**
 * @brief Task queue policies.
 *
 */
enum class TaskPolicy : uint8_t {
    FIFO, // First input => First output
    LIFO, // Last input => First output
};

} // namespace nexus::exec
