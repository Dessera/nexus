#include "nexus/error.hpp"
#include <string>

namespace nexus {

Error::Error(Code code, std::string &&msg)
    : _msg(std::move(msg)), _code(code) {}

Error::Error(Code code, const std::string &msg)
    : Error(code, std::string(msg)) {}

Error::Error(Code code, const char *msg) : _msg(msg), _code(code) {}

Error::Error(Code code) : Error(code, std::strerror(errno)) {}

auto Error::what() const noexcept -> const char * { return _msg.c_str(); }

} // namespace nexus
