#pragma once
#include "../config.h"

#define PLUGIN_LIST \
    PLUGIN_LEFT     \
    PLUGIN_CENTER   \
    PLUGIN_RIGHT

// fix unnecessary warnings
#define X(name) void* plugin_##name(void* arg);
PLUGIN_LIST
#undef X

// macros to define plugin counts

#define X(name) name##_idx,
typedef enum {
    PLUGIN_LIST
        PLUGIN_COUNT
} plugin_indexes;
#undef X

#define X(name) name##_left_idx,
typedef enum {
    PLUGIN_LEFT
        PLUGIN_LEFT_COUNT,
} plugin_left_indexes;
#undef X

#define X(name) name##_center_idx,
typedef enum {
    PLUGIN_CENTER
        PLUGIN_CENTER_COUNT,
} plugin_center_indexes;
#undef X

#define X(name) name##_right_idx,
typedef enum {
    PLUGIN_RIGHT
        PLUGIN_RIGHT_COUNT,
} plugin_right_indexes;
#undef X
