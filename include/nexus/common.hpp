#pragma once

#if defined _WIN32 || defined __CYGWIN__
    #ifdef BUILDING_NEXUS
        #define NEXUS_EXPORT __declspec(dllexport)
    #else
        #define NEXUS_EXPORT __declspec(dllimport)
    #endif
#else
    #ifdef BUILDING_NEXUS
        #define NEXUS_EXPORT __attribute__((visibility("default")))
    #else
        #define NEXUS_EXPORT
    #endif
#endif

#if defined _WIN32 || defined __CYGWIN__
    #define nexus_inline msvc::forceinline
#else
    #define nexus_inline gnu::always_inline
#endif

#define NEXUS_INLINE [[nexus_inline]] inline

#define NEXUS_COPY_DELETE(type)                                                \
    type(const type &other) = delete;                                          \
    auto operator=(const type &other)->type & = delete

#define NEXUS_COPY_DEFAULT(type)                                               \
    type(const type &other) = default;                                         \
    auto operator=(const type &other)->type & = default

#define NEXUS_MOVE_DELETE(type)                                                \
    type(type &&other) noexcept = delete;                                      \
    auto operator=(type &&other)->type & = delete

#define NEXUS_MOVE_DEFAULT(type)                                               \
    type(type &&other) noexcept = default;                                     \
    auto operator=(type &&other) noexcept -> type & = default
