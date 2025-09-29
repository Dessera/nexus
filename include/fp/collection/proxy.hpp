#pragma once

#include "fp/lazy.hpp"

#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace fp::collection {

template <typename T, typename LE>
    requires(ILazyResult<LE, T>)
class ProxyVec {
  public:
    using Type = std::decay_t<T>;

  private:
    std::unique_ptr<std::vector<Type>> _data; // Own this
    std::vector<LE> _proxy;

  public:
    ProxyVec(std::vector<LE> &&proxy, std::vector<Type> *data)
        : _data(data), _proxy(std::move(proxy)) {
        assert(proxy.size() == data->size());
    }
};

} // namespace fp::collection
