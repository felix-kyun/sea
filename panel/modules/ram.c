#include "colors.h"
#include "log.h"
#include "modules/modules.h"
#include "string-utf8.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>

#define RAM_ICON "  "
#define RAM_COLOR MAGENTA
#define MEM_FILE "/proc/meminfo"

static char buffer[32];

inline static void set_ram_usage(ModuleState* state, float used, float total)
{
    (void)total;
    snprintf(buffer, sizeof(buffer), " " RAM_COLOR RAM_ICON "%.1fGib " RESET, used);
    string_set_cstr(state->data, buffer);
    state->signal_render();
}

uint32_t get_total_ram(void)
{
    FILE* file = fopen(MEM_FILE, "r");
    if (!file) {
        logger_log(LOG_ERROR, "failed to open meminfo file: " MEM_FILE);
        return -1;
    }

    uint32_t total_kb = 0;
    char tmp_buffer[64];
    while (fgets(tmp_buffer, sizeof(tmp_buffer), file)) {
        if (sscanf(tmp_buffer, "MemTotal: %d kB", &total_kb) == 1) {
            break;
        }
    }

    fclose(file);
    return total_kb;
}

uint32_t get_available_ram(void)
{
    FILE* file = fopen(MEM_FILE, "r");
    if (!file) {
        logger_log(LOG_ERROR, "failed to open meminfo file: " MEM_FILE);
        return -1;
    }

    uint32_t available_kb = 0;
    char tmp_buffer[64];
    while (fgets(tmp_buffer, sizeof(tmp_buffer), file)) {
        if (sscanf(tmp_buffer, "MemAvailable: %d kB", &available_kb) == 1) {
            break;
        }
    }

    fclose(file);
    return available_kb;
}

void* module_init(void* _state)
{
    ModuleState* state = _state;
    uint32_t total_ram = get_total_ram();

    while (*state->running) {
        float used_ram = ((float)total_ram - get_available_ram()) / 1024 / 1024;
        float total_gb = (float)total_ram / 1024 / 1024;

        set_ram_usage(state, used_ram, total_gb);
        msleep(5000);
    }

    return NULL;
}
