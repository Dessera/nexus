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
    #define _nexus_inline __forceinline
#else
    #define _nexus_inline __attribute__((always_inline)) inline
#endif
