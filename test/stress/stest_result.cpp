
#include "nexus/utils/format.hpp"
#include "nexus/utils/result.hpp"

#include <bits/getopt_core.h>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <optional>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <variant>

namespace {

using nexus::Result;

using ValueType = double;
using ErrorType = int;

constexpr std::size_t FALLBACK_LOOP_CNT = 1000;

auto ecode_gen([[maybe_unused]] ValueType &value) -> ErrorType { return 1; }

auto eopt_gen() -> std::optional<ValueType> { return std::nullopt; }

auto evar_gen() -> std::variant<ValueType, ErrorType> { return ErrorType(); }

auto eres_gen() -> Result<ValueType, ErrorType> { return Err(1); }

auto throw_gen() -> ValueType { throw std::runtime_error("Exception!"); }

} // namespace

auto main(int argc, char **argv) -> int {
    namespace chrono = std::chrono;

    std::size_t loop_cnt = FALLBACK_LOOP_CNT;

    int opt = -1;
    while ((opt = getopt(argc, argv, "l::")) != -1) {
        switch (opt) {
        case 'l': try { loop_cnt = std::stoull(optarg);
            } catch (...) {
                nexus::println(stderr,
                               "Warning: Invalid loop count, use default");
            }
            break;
        default:
            nexus::println(stderr, "Warning: Unknown command {}",
                           static_cast<char>(opt));
            break;
        }
    }

    auto ecode_start = chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < loop_cnt; ++i) {
        ValueType                           value = 0;
        [[maybe_unused]] volatile ErrorType res = ecode_gen(value);
        if (res != 0) {
            value = 1;
        }
    }
    auto ecode_end = chrono::high_resolution_clock::now();

    auto eopt_start = chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < loop_cnt; ++i) {
        [[maybe_unused]] volatile ValueType res = eopt_gen().value_or(1);
    }
    auto eopt_end = chrono::high_resolution_clock::now();

    auto evar_start = chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < loop_cnt; ++i) {
        [[maybe_unused]] volatile ValueType value = 0;
        auto                                res = evar_gen();
        if (auto *pvalue = std::get_if<ValueType>(&res); pvalue != nullptr) {
            value = *pvalue;
        } else {
            value = 1;
        }
    }
    auto evar_end = chrono::high_resolution_clock::now();

    auto eres_start = chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < loop_cnt; ++i) {
        [[maybe_unused]] volatile ValueType res = eres_gen().unwrap_or(1);
    }
    auto eres_end = chrono::high_resolution_clock::now();

    auto throw_start = chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < loop_cnt; ++i) {
        [[maybe_unused]] volatile ValueType res = 0;
        try {
            res = throw_gen();
        } catch (...) {
            res = 1;
        }
    }
    auto throw_end = chrono::high_resolution_clock::now();

    chrono::duration<double> ecode_time = ecode_end - ecode_start;
    chrono::duration<double> eopt_time = eopt_end - eopt_start;
    chrono::duration<double> evar_time = evar_end - evar_start;
    chrono::duration<double> eres_time = eres_end - eres_start;
    chrono::duration<double> throw_time = throw_end - throw_start;

    nexus::println();
    nexus::println("Statistics:");
    nexus::println("  Loop count: {}", loop_cnt);
    nexus::println("  Error code: {}", ecode_time);
    nexus::println("  Optional  : {}", eopt_time);
    nexus::println("  Variant   : {}", evar_time);
    nexus::println("  Result    : {}", eres_time);
    nexus::println("  Throw     : {}", throw_time);
}
