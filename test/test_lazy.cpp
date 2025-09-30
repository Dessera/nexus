#include <nexus/lazy.hpp>

#include <cassert>
#include <memory>

auto main() -> int {
    auto sum = nexus::lazy_eval([]() { return 1 + 2; });
    assert(*sum == 3);

    auto dep_value = nexus::lazy_eval_rc([]() { return 42; }); // NOLINT
    auto fin_value =
        nexus::lazy_eval([dep_value]() { return **dep_value + 1; });
    assert(*fin_value == 43);

    auto mulcall = nexus::lazy_eval([]() {
        static int cnt = 0;
        cnt++;
        return cnt;
    });

    assert(*mulcall == 1);
    assert(*mulcall == 1);
}
