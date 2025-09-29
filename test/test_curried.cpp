#include <fp/curried.hpp>

#include <cassert>
#include <functional>

namespace {

auto addref(int &lhs, int &rhs) -> int { return lhs + rhs; }

} // namespace

auto main() -> int {
    auto cadd = fp::make_curried(std::plus<>());
    assert(cadd(1)(2) == 3);

    auto cadd_five = cadd(5); // NOLINT
    assert(cadd_five(4) == 9);
    assert(cadd_five(5) == 10);

    int lhs = 4;
    int rhs = 2;

    auto pure_addref = fp::make_curried(addref);
    static_assert(
        !std::is_same_v<decltype(pure_addref(lhs)(rhs)), int>); // Not invokable
}
