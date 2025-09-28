#include <cassert>
#include <fp/curried.hpp>
#include <functional>

namespace {

auto addref(int &lhs, int &rhs) -> int { return lhs + rhs; }

} // namespace

auto main() -> int {
  auto cadd = fp::make_pure_curried(std::plus<>());
  assert(cadd(1)(2) == 3);

  auto cadd_five = cadd(5); // NOLINT
  assert(cadd_five(4) == 9);
  assert(cadd_five(5) == 10);

  int lhs = 4;
  int rhs = 2;
  auto impure_addref = fp::make_impure_curried(addref);
  assert(impure_addref(lhs)(rhs) ==
         6); // No constexpr because mutable reference.

  auto pure_addref = fp::make_pure_curried(addref);
  static_assert(
      !std::is_same_v<decltype(pure_addref(lhs)(rhs)), int>); // Not invokable
}
