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
        int minutes = uptime / 60;

        int update_interval = from_hours(1);
        if (days == 0 && hours == 0) {
            // if less than one hour
            // show minute
            snprintf(buffer, 64, "%s%s" UPTIME_ICON "%dm", color, background, minutes);
            update_interval = from_minutes(1);
        } else if (days == 0) {
            // if less than one day
            // show hour & minute
            snprintf(buffer, 64, "%s%s" UPTIME_ICON "%dh %dm", color, background, hours, minutes);
            update_interval = from_minutes(1);
        } else {
            // show day & hour
            snprintf(buffer, 64, "%s%s" UPTIME_ICON "%dd %dh", color, background, days, hours);
        }

        if (!string_equals_cstr(state->data, buffer)) {
            string_set_cstr(state->data, buffer);
            state->signal_render();
        }

        msleep(update_interval);
    }

    return NULL;
}
