#pragma once
#include <stdbool.h>

// used to control panel state
// defined in panel.c
extern bool running;

// signal to stop panel
// defined in panel.c
void panel_stop(void);
