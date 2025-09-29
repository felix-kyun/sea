#pragma once
#include "../config.h"

#define MODULE_LIST \
    LEFT            \
        CENTER      \
            RIGHT

// fix unnecessary warnings
#define X(name) void* module_##name(void* arg);
MODULE_LIST
#undef X

// macros to define plugin counts

#define X(name) name##_idx,
typedef enum {
    MODULE_LIST
        MODULE_COUNT
} module_indexes;
#undef X

#define X(name) name##_left_idx,
typedef enum {
    LEFT
        LEFT_COUNT,
} module_left_indexes;
#undef X

#define X(name) name##_center_idx,
typedef enum {
    CENTER
        CENTER_COUNT,
} module_center_indexes;
#undef X

#define X(name) name##_right_idx,
typedef enum {
    RIGHT
        RIGHT_COUNT,
} module_right_indexes;
#undef X
