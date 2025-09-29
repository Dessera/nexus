#include <fp/lazy.hpp>

#include <cassert>

auto main() -> int {
    auto sum = fp::lazy_eval([]() { return 1 + 2; });
    assert(*sum == 3);

    auto dep_value = fp::lazy_eval_rc([]() { return 42; }); // NOLINT
    auto fin_value = fp::lazy_eval([dep_value]() { return **dep_value + 1; });
    assert(*fin_value == 43);

    auto mulcall = fp::lazy_eval([]() {
        static int cnt = 0;
        cnt++;
        return cnt;
    });

    assert(*mulcall == 1);
    assert(*mulcall == 1);
}
