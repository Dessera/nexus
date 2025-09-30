#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace nexus {

/**
 * @brief Curried function wrapper.
 *
 * @tparam F Function-like object type.
 * @tparam FixedArgs Saved arguments type.
 */
template <typename F, typename... FixedArgs> class Curried {
  private:
    /**
     * @brief Decayed arguments tuple type.
     *
     * @tparam Args Tuple element types.
     */
    template <typename... Args>
    using ArgsTuple = std::tuple<std::decay_t<Args>...>;

    template <typename InvokeF, typename... Args>
    constexpr static bool IsInvokableV =
        std::is_invocable_v<InvokeF, std::decay_t<Args>...>;

  public:
    /**
     * @brief Decayed function type for storage.
     *
     */
    using FixedFunc = std::decay_t<F>;

    /**
     * @brief Decayed fixed arguments tuple type.
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
        requires(IsInvokableV<F, FixedArgs..., Args...>)
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
        requires(!IsInvokableV<F, FixedArgs..., Args...>)
    {
        return Curried<F, FixedArgs..., Args...>(
            _func, std::tuple_cat(
                       _args, ArgsTuple<Args...>(std::forward<Args>(args)...)));
    }
};

/**
 * @brief Create curried function.
 *
 * @tparam F Function-like object type.
 * @tparam Args Saved arguments type.
 * @param func Function-like object.
 * @param args Saved arguments.
 * @return Curried<F, Args...> Curried function object.
 */
template <typename F, typename... Args>
constexpr auto make_curried(F &&func, Args &&...args) -> decltype(auto) {
    return Curried<F, Args...>{std::forward<F>(func),
                               std::forward<Args>(args)...};
}

} // namespace nexus
