#pragma once

#include "fp/policy.hpp"

#include <tuple>
#include <type_traits>

namespace fp::detail {

template <Policy P> struct ArgsDecayHelper {
    template <typename T> using WrapArgs = std::decay_t<T>;
};

template <> struct ArgsDecayHelper<Policy::Impure> {
    template <typename T> using WrapArgs = T;
};

template <Policy P> struct ArgsHelper {
    template <typename T>
    using WrapArgs = ArgsDecayHelper<P>::template WrapArgs<T>;

    template <typename... Args> using Tuple = std::tuple<WrapArgs<Args>...>;

    template <typename InvokeF, typename... Args>
    using InvokeResult = std::invoke_result<InvokeF, WrapArgs<Args>...>;

    template <typename InvokeF, typename... Args>
    using InvokeResultT = std::invoke_result_t<InvokeF, WrapArgs<Args>...>;

    template <typename InvokeF, typename... Args>
    using IsInvokable = std::is_invocable<InvokeF, WrapArgs<Args>...>;

    template <typename InvokeF, typename... Args>
    constexpr static bool IsInvokableV =
        std::is_invocable_v<InvokeF, WrapArgs<Args>...>;
};

} // namespace fp::detail
