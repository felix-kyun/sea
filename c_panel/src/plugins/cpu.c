#include "colors.h"
#include "log.h"
#include "plugins/plugins.h"
#include "state.h"
#include "utils.h"
#include <stdio.h>

#define STAT_FILE "/proc/stat"
#define CPU_ICON " "
#define CPU_COLOR CYAN

static unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
static char buffer[256];

static void load_cpu_info(void)
{
    FILE* stat = fopen(STAT_FILE, "r");

    if (fgets(buffer, sizeof(buffer), stat) == NULL) {
        logger_log(LOG_ERROR, "failed to read /proc/stat");
    }

    sscanf(buffer, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
        &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

    fclose(stat);
}

static inline unsigned long long get_cpu_total_time(void)
{
    return user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
}

static inline unsigned long long get_cpu_idle_time(void)
{
    return idle + iowait;
}

static inline void set_cpu_usage(PluginState* state, int usage)
{
    snprintf(buffer, sizeof(buffer), " " CPU_COLOR CPU_ICON "%d%% " RESET, usage);
    string_set_cstr(state->data, buffer);
    panel_signal_render();
}

void* plugin_cpu(void* _state)
{
    PluginState* state = _state;
    set_cpu_usage(state, 0);

    while (running) {
        load_cpu_info();

        unsigned long long total = get_cpu_total_time();
        unsigned long long used = total - get_cpu_idle_time();

        msleep(5000);
        load_cpu_info();

        unsigned long long new_total = get_cpu_total_time();
        unsigned long long new_used = new_total - get_cpu_idle_time();

        unsigned long long diff_total = new_total - total;
        unsigned long long diff_used = new_used - used;

        if (diff_total == 0) {
            logger_log(LOG_WARN, "diff total is zero, skipping cpu usage calculation");
            continue;
        }

        int cpu_usage = (int)((diff_used * 100) / diff_total);
        set_cpu_usage(state, cpu_usage);
    }

    return NULL;
}
