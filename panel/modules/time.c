#include "colors.h"
#include "modules/modules.h"
#include "string-utf8.h"
#include "utils.h"
#include <stdio.h>
#include <time.h>

void* module_init(void* _state)
{
    ModuleState* state = _state;
    const char* color = get_fg_color(state->config_get(state->name, "color"), "yellow");
    const char* background = get_bg_color(state->config_get(state->name, "background"), "default");
    char time_buffer[64];
    char buffer[128];

    while (*state->running) {
        time_t now = time(NULL);
        struct tm* t = localtime(&now);
        strftime(time_buffer, 64, " %H:%M:%S", t);

        snprintf(buffer, 128, "%s%s%s", color, background, time_buffer);

        // only signal render if the time has changed
        if (!string_equals_cstr(state->data, buffer)) {
            string_set_cstr(state->data, buffer);
            state->signal_render();
        }

        msleep(1000);
    }

    return NULL;
}
