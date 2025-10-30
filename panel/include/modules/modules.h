#pragma once
#include "string-utf8.h"
#include <stdbool.h>

void* module_init(void* context);

struct ModuleState;

typedef void (*cleanup_func)(void);
typedef void (*event_handler)(struct ModuleState*);

typedef struct ModuleState {
    const char* name;

    bool* running;
    void (*signal_render)(void);
    char* (*config_get)(const char* section, const char* key);

    // used for custom modules
    void* custom_data;

    string* data;
    cleanup_func cleanup;

    event_handler on_left_click;
    event_handler on_right_click;
    event_handler on_middle_click;
    event_handler on_scroll_up;
    event_handler on_scroll_down;
} ModuleState;
