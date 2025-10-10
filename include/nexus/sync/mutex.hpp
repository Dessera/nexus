#pragma once

#include "nexus/common.hpp"

#include <chrono>
#include <mutex>
#include <type_traits>
#include <utility>

namespace nexus::sync {

/**
 * @brief Mutex lock with resource ownership.
 *
 * @tparam T Resource object type.
 * @tparam Lck Lock type.
 * @tparam Grd Guard type.
 */
template <typename T, typename Lck = std::mutex,
          typename Grd = std::unique_lock<Lck>>
    requires(!std::is_reference_v<T>)
class Mutex {
  public:
    using ObjectType = T;
    using LockType = Lck;
    using GuardType = Grd;

    /**
     * @brief Object guard type.
     *
     */
    class Guard {
      private:
        ObjectType *_obj;
        GuardType   _guard;

      public:
        template <typename... Args>
        Guard(ObjectType *obj, LockType &lock, Args &&...args)
            : _obj(obj), _guard(lock, std::forward<Args>(args)...) {}

        ~Guard() = default;

        NEXUS_COPY_DELETE(Guard);
        NEXUS_MOVE_DEFAULT(Guard);

        [[nodiscard]] NEXUS_INLINE auto operator*() { return *get(); }
        [[nodiscard]] NEXUS_INLINE auto operator*() const { return *get(); }

        [[nodiscard]] NEXUS_INLINE auto operator->() { return get(); }
        [[nodiscard]] NEXUS_INLINE auto operator->() const { return get(); }

        [[nodiscard]] NEXUS_INLINE auto get() { return _obj; }
        [[nodiscard]] NEXUS_INLINE auto get() const { return _obj; }

        void lock() { _guard.lock(); }

        auto try_lock() -> bool { return _guard.try_lock(); }

        template <class Rep, class Period>
        auto try_lock_for(const std::chrono::duration<Rep, Period> &timeout)
            -> bool {
            return _guard.try_lock_for(timeout);
        }

        template <class Clock, class Duration>
        auto
        try_lock_until(const std::chrono::time_point<Clock, Duration> &timeout)
            -> bool {
            return _guard.try_lock_until(timeout);
        }

        void unlock() { _guard.unlock(); }
    };

  private:
    ObjectType _obj;
    LockType   _lock{};

  public:
    template <typename... Args>
    Mutex(Args &&...args) : _obj(std::forward<Args>(args)...) {}

    ~Mutex() = default;

    NEXUS_COPY_DELETE(Mutex);
    NEXUS_MOVE_DELETE(Mutex);

    /**
     * @brief Get the ownership of object.
     *
     * @tparam Args Guard arguments type.
     * @param args Extra arguments for GuardType.
     * @return Guard Object guard.
     */
    template <typename... Args>
    NEXUS_INLINE auto lock(Args &&...args) -> Guard {
        return Guard(&_obj, _lock, std::forward<Args>(args)...);
    }
};

} // namespace nexus::sync
