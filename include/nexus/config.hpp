#pragma once

#include "nexus/.config" // IWYU pragma: keep

#ifdef NEXUS_TESTS_DISABLE_DELAY
    #define NEXUS_TESTS_DELAY(name) DISABLED_##name
#else
    #define NEXUS_TESTS_DELAY(name) name
#endif
