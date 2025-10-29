#pragma once
#include "string-utf8.h"
#include <stdbool.h>

void* module_init(void* context);

struct ModuleState;

typedef void (*cleanup_func)(void);
typedef void (*event_handler)(struct ModuleState*);

typedef struct ModuleState {
    string* data;
    cleanup_func cleanup;
    event_handler on_left_click;
    event_handler on_right_click;
    event_handler on_middle_click;
    event_handler on_scroll_up;
    event_handler on_scroll_down;
} ModuleState;

// used to signal render to render the panel again
// defined in panel.c
void panel_signal_render(void);
