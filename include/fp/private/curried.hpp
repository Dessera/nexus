#pragma once

#include "fp/policy.hpp"

#include <type_traits>

namespace fp::detail {

template <Policy P> struct CurriedHelper {
  template <typename T> using ArgsWrapper = std::decay_t<T>;
};

template <> struct CurriedHelper<Policy::Impure> {
  template <typename T> using ArgsWrapper = T;
};

} // namespace fp::detail
