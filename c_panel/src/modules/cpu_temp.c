#include "colors.h"
#include "log.h"
#include "modules/modules.h"
#include "state.h"
#include "utils.h"
#include <stdio.h>

#define TEMP_ICON " "
#define TEMP_COLOR RED
#define TEMP_FILE "/sys/class/thermal/thermal_zone0/temp"

static char buffer[32];

inline static void set_cpu_temp(ModuleState* state, int temp)
{
    snprintf(buffer, sizeof(buffer), " " TEMP_COLOR TEMP_ICON "%d°C " RESET, temp);
    string_set_cstr(state->data, buffer);
    panel_signal_render();
}

int get_cpu_temp(void)
{
    FILE* file = fopen(TEMP_FILE, "r");
    if (!file) {
        logger_log(LOG_ERROR, "failed to open cpu temp file: " TEMP_FILE);
        return -1;
    }

    int temp = 0;
    if (fscanf(file, "%d", &temp) != 1) {
        logger_log(LOG_ERROR, "failed to read cpu temp value");
        fclose(file);
        return -1;
    }

    fclose(file);
    return temp / 1000;
}

void* module_cpu_temp(void* _state)
{
    ModuleState* state = _state;

    while (running) {
        int temp = get_cpu_temp();
        if (temp != -1) {
            set_cpu_temp(state, temp);
        }
        msleep(5000);
    }

    return NULL;
}
