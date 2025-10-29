#include "colors.h"
#include "log.h"
#include "modules/modules.h"
#include "state.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>

#define STAT_FILE "/proc/stat"
#define CPU_ICON " "
#define CPU_COLOR CYAN

static uint64_t user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
static char buffer[256];

static void load_cpu_info(void)
{
    FILE* stat = fopen(STAT_FILE, "r");

    if (fscanf(stat, "cpu  %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
            &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice)
        != 10) {
        logger_log(LOG_ERROR, "failed to read cpu info from /proc/stat");
    }

    fclose(stat);
}

static inline uint64_t get_cpu_total_time(void)
{
    return user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
}

static inline uint64_t get_cpu_idle_time(void)
{
    return idle + iowait;
}

static inline void set_cpu_usage(ModuleState* state, int usage)
{
    snprintf(buffer, sizeof(buffer), " " CPU_COLOR CPU_ICON "%d%% " RESET, usage);
    string_set_cstr(state->data, buffer);
    panel_signal_render();
}

void* module_init(void* _state)
{
    ModuleState* state = _state;
    set_cpu_usage(state, 0);

    while (running) {
        load_cpu_info();

        uint64_t total = get_cpu_total_time();
        uint64_t used = total - get_cpu_idle_time();

        msleep(5000);
        load_cpu_info();

        uint64_t new_total = get_cpu_total_time();
        uint64_t new_used = new_total - get_cpu_idle_time();

        uint64_t diff_total = new_total - total;
        uint64_t diff_used = new_used - used;

        if (diff_total == 0) {
            logger_log(LOG_WARN, "diff total is zero, skipping cpu usage calculation");
            continue;
        }

        int cpu_usage = (int)((diff_used * 100) / diff_total);
        set_cpu_usage(state, cpu_usage);
    }

    return NULL;
}
