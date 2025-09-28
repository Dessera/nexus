#pragma once

#include <cstdint>

namespace fp {

/**
 * @brief FP implemetation policy.
 *
 */
enum class Policy : uint8_t {
  Pure,   // Disallow (maybe) mutable operaions.
  Impure, // Allow all c++ operations.
};

} // namespace fp
