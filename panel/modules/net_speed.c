#include "colors.h"
#include "log.h"
#include "modules/modules.h"
#include "state.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define STAT_FILE "/proc/net/dev"
#define NET_UP_ICON " "
#define NET_DOWN_ICON " "
#define NET_COLOR BLUE
#define NET_REFRESH 2

void inline static set_net_speed(ModuleState* state, int down_kib, int up_kib)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), " " NET_COLOR NET_UP_ICON "%dKiB " NET_DOWN_ICON "%dKiB " RESET, up_kib, down_kib);
    string_set_cstr(state->data, buffer);
    state->signal_render();
}

void static read_net_stat(int* down_kib, int* up_kib)
{
    FILE* file = fopen(STAT_FILE, "r");
    *down_kib = 0;
    *up_kib = 0;

    if (!file) {
        logger_log(LOG_ERROR, "failed to open net dev file: " STAT_FILE);
        return;
    }

    while (!feof(file)) {
        char iface[32];
        uint64_t rx_bytes, tx_bytes;
        if (fscanf(file, "%31s %lu %*u %*u %*u %*u %*u %*u %*u %lu", iface, &rx_bytes, &tx_bytes) == 3) {
            if (strcmp(iface, "lo:") != 0) {
                *down_kib += rx_bytes / 1024;
                *up_kib += tx_bytes / 1024;
            }
        }
    }
    fclose(file);
}

void* module_init(void* _state)
{
    ModuleState* state = _state;
    set_net_speed(state, 0, 0);

    while (*state->running) {
        int down1, up1, down2, up2;
        read_net_stat(&down1, &up1);
        msleep(NET_REFRESH * 1000);
        read_net_stat(&down2, &up2);

        int down_speed = (down2 - down1) / NET_REFRESH;
        int up_speed = (up2 - up1) / NET_REFRESH;

        set_net_speed(state, down_speed, up_speed);
    }

    return NULL;
}
