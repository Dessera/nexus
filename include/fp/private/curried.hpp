#pragma once

#include "fp/policy.hpp"

#include <tuple>
#include <type_traits>

namespace fp::detail {

template <Policy P> struct CurriedWrapHelper {
  template <typename T> using WrapArgs = std::decay_t<T>;
};

template <> struct CurriedWrapHelper<Policy::Impure> {
  template <typename T> using WrapArgs = T;
};

template <Policy P> struct CurriedHelper {
  template <typename T>
  using WrapArgs = CurriedWrapHelper<P>::template WrapArgs<T>;

  template <typename... Args> using ArgsTuple = std::tuple<WrapArgs<Args>...>;

  template <typename InvokeF, typename... Args>
  constexpr static bool IsInvokableV =
      std::is_invocable_v<InvokeF, WrapArgs<Args>...>;
};

} // namespace fp::detail
