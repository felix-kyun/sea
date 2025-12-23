#include "colors.h"
#include "log.h"
#include "modules/modules.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/sysinfo.h>

#define UPTIME_ICON " "
#define UPTIME_COLOR CYAN
#define UPTIME_REFRESH 60000 // 60 seconds

static char* color = "";
static char* background = "";
static char buffer[64];

static void get_uptime(int* days, int* hours, int* minutes)
{
    struct sysinfo info;
    if (sysinfo(&info) != 0) {
        logger_log(LOG_ERROR, "failed to get system info");
        *days = 0;
        *hours = 0;
        *minutes = 0;
        return;
    }

    long uptime_seconds = info.uptime;
    *days = uptime_seconds / 86400;
    *hours = (uptime_seconds % 86400) / 3600;
    *minutes = (uptime_seconds % 3600) / 60;
}

static void set_uptime_display(ModuleState* state, int days, int hours, int minutes)
{
    if (days > 0) {
        snprintf(buffer, sizeof(buffer), "%s%s" UPTIME_ICON "%dd %dh %dm",
            background, color, days, hours, minutes);
    } else if (hours > 0) {
        snprintf(buffer, sizeof(buffer), "%s%s" UPTIME_ICON "%dh %dm",
            background, color, hours, minutes);
    } else {
        snprintf(buffer, sizeof(buffer), "%s%s" UPTIME_ICON "%dm",
            background, color, minutes);
    }
    string_set_cstr(state->data, buffer);
    state->signal_render();
}

void* module_init(void* _state)
{
    ModuleState* state = _state;
    color = get_fg_color(state->config_get(state->name, "color"), "cyan");
    background = get_bg_color(state->config_get(state->name, "background"), "default");

    int days, hours, minutes;

    while (*state->running) {
        get_uptime(&days, &hours, &minutes);
        set_uptime_display(state, days, hours, minutes);
        msleep(UPTIME_REFRESH);
    }

    return NULL;
}
