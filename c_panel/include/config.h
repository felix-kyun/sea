#pragma once

#define PLUGIN_LIST \
    X(date_time)    \
    X(text)         \
    X(auto_kill)

// fix unnecessary warnings
#define X(name) void* plugin_##name(void* arg);
PLUGIN_LIST
#undef X

// get plugin count
#define X(name) name##_idx,
typedef enum {
    PLUGIN_LIST
        PLUGIN_COUNT
} plugin_indexes;
#undef X
