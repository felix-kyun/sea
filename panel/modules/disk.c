#include "colors.h"
#include "log.h"
#include "modules/modules.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/statvfs.h>

#define DISK_ICON " "
#define DISK_COLOR YELLOW
#define DISK_PATH "/"
#define DISK_REFRESH 30000 // 30 seconds

static char* color = "";
static char* background = "";
static char buffer[64];

static void get_disk_usage(uint64_t* total_gb, uint64_t* used_gb, float* percentage)
{
    struct statvfs stat;

    if (statvfs(DISK_PATH, &stat) != 0) {
        logger_log(LOG_ERROR, "failed to get filesystem stats for %s", DISK_PATH);
        *total_gb = 0;
        *used_gb = 0;
        *percentage = 0.0f;
        return;
    }

    uint64_t total_bytes = stat.f_blocks * stat.f_frsize;
    uint64_t available_bytes = stat.f_bavail * stat.f_frsize;
    uint64_t used_bytes = total_bytes - available_bytes;

    *total_gb = total_bytes / (1024ULL * 1024ULL * 1024ULL);
    *used_gb = used_bytes / (1024ULL * 1024ULL * 1024ULL);
    *percentage = (float)used_bytes / (float)total_bytes * 100.0f;
}

static char* get_disk_color(float percentage)
{
    if (percentage >= 90.0f) {
        return RED;
    } else if (percentage >= 75.0f) {
        return MAGENTA;
    } else if (percentage >= 50.0f) {
        return YELLOW;
    } else {
        return GREEN;
    }
}

static void set_disk_usage(ModuleState* state, uint64_t used_gb, uint64_t total_gb, float percentage)
{
    char* usage_color = get_disk_color(percentage);
    snprintf(buffer, sizeof(buffer), "%s%s" DISK_ICON " %s%luG/%luG (%.0f%%)",
        background, color, usage_color, used_gb, total_gb, percentage);
    string_set_cstr(state->data, buffer);
    state->signal_render();
}

void* module_init(void* _state)
{
    ModuleState* state = _state;
    color = get_fg_color(state->config_get(state->name, "color"), "yellow");
    background = get_bg_color(state->config_get(state->name, "background"), "default");

    uint64_t total_gb, used_gb;
    float percentage;

    while (*state->running) {
        get_disk_usage(&total_gb, &used_gb, &percentage);
        set_disk_usage(state, used_gb, total_gb, percentage);
        msleep(DISK_REFRESH);
    }

    return NULL;
}
