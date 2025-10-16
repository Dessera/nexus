#include "nexus/error.hpp"

#include <string>
#include <utility>

namespace nexus {

auto Error::to_std() -> ThrowableError { return std::move(*this); }

auto ThrowableError::what() const noexcept -> const char * {
    return _err.msg().c_str();
}

} // namespace nexus
