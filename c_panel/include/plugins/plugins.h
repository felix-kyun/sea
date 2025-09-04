#pragma once
#include "string-utf8.h"
#include <stdbool.h>

typedef struct PluginState {
    string* data;
} PluginState;

// used to signal render to render the panel again
// defined in panel.c
void panel_signal_render(void);
