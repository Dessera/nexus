#pragma once

#include "fp/policy.hpp"
#include "fp/private/args.hpp"

#include <tuple>
#include <type_traits>
#include <utility>

namespace fp {

/**
 * @brief Curried function wrapper.
 *
 * @tparam P Policy that restricts the passing of arguments.
 * @tparam F Function-like object type.
 * @tparam FixedArgs Saved arguments type.
 */
template <Policy P, typename F, typename... FixedArgs> class Curried {
  private:
    /**
     * @brief Private utils for Curried.
     *
     */
    using Helper = detail::ArgsHelper<P>;

    /**
     * @brief Redeclaration for Helper::ArgsTuple, only for convenience.
     *
     * @tparam Args Tuple element types.
     */
    template <typename... Args>
    using ArgsTuple = Helper::template Tuple<Args...>;

  public:
    /**
     * @brief Fixed function type (remove reference and others by std::decay).
     *
     */
    using FixedFunc = std::decay_t<F>;

    /**
     * @brief Fixed argument types (apply std::decay in pure policy).
     *
     */
    using FixedArgsTuple = ArgsTuple<FixedArgs...>;

  private:
    FixedFunc _func;
    FixedArgsTuple _args;

  public:
    /**
     * @brief Construct a new Curried with fixed arguments tuple.
     *
     * @param func Function-like object.
     * @param args Arguments tuple.
     */
    constexpr Curried(FixedFunc &&func, FixedArgsTuple &&args)
        : _func(std::move(func)), _args(std::move(args)) {}

    /**
     * @brief Construct a new Curried with fixed arguments tuple.
     *
     * @param func Function-like object.
     * @param args Arguments tuple.
     */
    constexpr Curried(const FixedFunc &func, FixedArgsTuple &&args)
        : _func(func), _args(std::move(args)) {}

    /**
     * @brief Construct a new Curried with fixed arguments.
     *
     * @param func Function-like object.
     * @param args Arguments.
     */
    constexpr Curried(FixedFunc &&func, FixedArgs &&...args)
        : Curried(std::move(func),
                  FixedArgsTuple(std::forward<FixedArgs>(args)...)) {}

    /**
     * @brief Construct a new Curried with fixed arguments.
     *
     * @param func Function-like object.
     * @param args Arguments.
     */
    constexpr Curried(const FixedFunc &func, FixedArgs &&...args)
        : Curried(func, FixedArgsTuple(std::forward<FixedArgs>(args)...)) {}

    /**
     * @brief Function entry when arguments are ready.
     *
     * @tparam Args Final arguments type.
     */
    template <typename... Args>
    constexpr auto operator()(Args &&...args) const -> decltype(auto)
        requires(Helper::template IsInvokableV<F, FixedArgs..., Args...>)
    {
        return std::apply(
            _func, std::tuple_cat(
                       _args, ArgsTuple<Args...>(std::forward<Args>(args)...)));
    }

    /**
     * @brief Function entry when arguments are not ready.
     *
     * @tparam Args New arguments type.
     */
    template <typename... Args>
    constexpr auto operator()(Args &&...args) const -> decltype(auto)
        requires(!Helper::template IsInvokableV<F, FixedArgs..., Args...>)
    {
        return Curried<P, F, FixedArgs..., Args...>(
            _func, std::tuple_cat(
                       _args, ArgsTuple<Args...>(std::forward<Args>(args)...)));
    }
};

/**
 * @brief Pure mode curried wrapper.
 *
 * @tparam F Function-like object.
 * @tparam FixedArgs Saved arguments.
 */
template <typename F, typename... FixedArgs>
using PureCurried = Curried<Policy::Pure, F, FixedArgs...>;

/**
 * @brief Impure mode curried wrapper.
 *
 * @tparam F Function-like object.
 * @tparam FixedArgs Saved arguments.
 */
template <typename F, typename... FixedArgs>
using ImpureCurried = Curried<Policy::Impure, F, FixedArgs...>;

/**
 * @brief Create curried function.
 *
 * @tparam P Policy that restricts the passing of arguments.
 * @tparam F Function-like object type.
 * @tparam Args Saved arguments type.
 * @param func Function-like object.
 * @param args Saved arguments.
 * @return Curried<P, F, Args...> Curried function object.
 */
template <Policy P, typename F, typename... Args>
constexpr auto make_curried(F &&func, Args &&...args)
    -> Curried<P, F, Args...> {
    return Curried<P, F, Args...>{std::forward<F>(func),
                                  std::forward<Args>(args)...};
}

/**
 * @brief Create pure curried function.
 *
 * @tparam F Function-like object type.
 * @tparam Args Saved arguments type.
 * @param func Function-like object.
 * @param args Saved arguments.
 * @return PureCurried<F, Args...> PureCurried function object.
 */
template <typename F, typename... Args>
constexpr auto make_pure_curried(F &&func, Args &&...args)
    -> PureCurried<F, Args...> {
    return make_curried<Policy::Pure>(std::forward<F>(func),
                                      std::forward<Args>(args)...);
}

/**
 * @brief Create impure curried function.
 *
 * @tparam F Function-like object type.
 * @tparam Args Saved arguments type.
 * @param func Function-like object.
 * @param args Saved arguments.
 * @return ImpureCurried<F, Args...> ImpureCurried function object.
 */
template <typename F, typename... Args>
constexpr auto make_impure_curried(F &&func, Args &&...args)
    -> ImpureCurried<F, Args...> {
    return make_curried<Policy::Impure>(std::forward<F>(func),
                                        std::forward<Args>(args)...);
}

} // namespace fp
