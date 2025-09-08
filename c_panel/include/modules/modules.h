#pragma once
#include "string-utf8.h"
#include <stdbool.h>

typedef void (*cleanup_func)(void);

typedef struct ModuleState {
    string* data;
    cleanup_func cleanup;
} ModuleState;

// used to signal render to render the panel again
// defined in panel.c
void panel_signal_render(void);
