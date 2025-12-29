#include "colors.h"
#include "modules/modules.h"
#include "utils.h"
#include <stdio.h>
#include <sys/sysinfo.h>

#define hour_secs (60 * 60)
#define day_secs (24 * hour_secs)
#define UPTIME_ICON " "

void* module_init(void* _state)
{
    ModuleState* state = _state;
    const char* color = get_module_fg_color(state, "green");
    const char* background = get_module_bg_color(state);

    struct sysinfo info;

    static char buffer[64];
    while (*state->running) {
        sysinfo(&info);
        int uptime = info.uptime;

        int days = uptime / day_secs;
        uptime %= day_secs;
        int hours = uptime / hour_secs;
        uptime %= hour_secs;

        snprintf(buffer, 64, "%s%s" UPTIME_ICON "%dd %dh", color, background, days, hours);
        if (!string_equals_cstr(state->data, buffer)) {
            string_set_cstr(state->data, buffer);
            state->signal_render();
        }

        msleep(1000);
    }

    return NULL;
}
