#pragma once

#include <memory>
#include <utility>

namespace nexus {

/**
 * @brief Unique ptr creator for types cannot be created in stack.
 *
 * @tparam T Target type.
 */
template <typename T> struct UniqueCreate {
    template <typename... Args>
    auto create_unique(Args &&...args) -> std::unique_ptr<T> {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
};

/**
 * @brief Shared ptr creator for types cannot be created in stack.
 *
 * @tparam T Target type.
 */
template <typename T> struct SharedCreate {
    template <typename... Args>
    auto create_shared(Args &&...args) -> std::shared_ptr<T> {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
};

} // namespace nexus
