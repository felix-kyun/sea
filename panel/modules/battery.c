#include "colors.h"
#include "log.h"
#include "modules/modules.h"
#include "string-utf8.h"
#include "utils.h"
#include <dirent.h>
#include <linux/limits.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SOURCE_DIR "/sys/class/power_supply"

static char battery_path[PATH_MAX] = { 0 };
static char battery_full_path[PATH_MAX] = { 0 };
static char battery_now_path[PATH_MAX] = { 0 };
static char battery_status_path[PATH_MAX] = { 0 };
static char* background = "";

typedef enum BatteryStatus {
    BATTERY_STATUS_UNKNOWN = 0,
    BATTERY_STATUS_CHARGING,
    BATTERY_STATUS_DISCHARGING,
    BATTERY_STATUS_NOT_CHARGING,
    BATTERY_STATUS_FULL,
} BatteryStatus;

static void init_battery_paths(const char* default_name)
{
    if (default_name != NULL) {
        snprintf(battery_path, PATH_MAX, "%s/%s", SOURCE_DIR, default_name);
        if (access(battery_path, F_OK) != 0) {
            logger_log(LOG_ERROR, "configured battery source '%s' not found", default_name);
            logger_log(LOG_ERROR, "falling back to automatic battery detection");
            pthread_exit(NULL);
        }
    } else {
        // check for files starting with "BAT" in SOURCE_DIR
        struct dirent* entry;
        DIR* dp = opendir(SOURCE_DIR);

        if (dp == NULL) {
            logger_log(LOG_ERROR, "failed to open directory: " SOURCE_DIR);
            return;
        }

        while ((entry = readdir(dp))) {
            if (strncmp(entry->d_name, "BAT", 3) == 0) {
                snprintf(battery_path, PATH_MAX, "%s%s", SOURCE_DIR, entry->d_name);
                closedir(dp);
                break;
            }
        }

        closedir(dp);
    }

    if (strlen(battery_path) == 0) {
        logger_log(LOG_ERROR, "No battery source found");
        pthread_exit(NULL);
    }

    logger_log(LOG_DEBUG, "using battery source: %s", battery_path);

    snprintf(battery_full_path, PATH_MAX, "%s/energy_full", battery_path);
    snprintf(battery_now_path, PATH_MAX, "%s/energy_now", battery_path);
    snprintf(battery_status_path, PATH_MAX, "%s/status", battery_path);

    logger_log(LOG_DEBUG, "battery paths initialized");
    logger_log(LOG_DEBUG, "full: %s", battery_full_path);
    logger_log(LOG_DEBUG, "now: %s", battery_now_path);
    logger_log(LOG_DEBUG, "status: %s", battery_status_path);
}

unsigned int get_battery_full(void)
{
    FILE* file = fopen(battery_full_path, "r");
    if (!file) {
        logger_log(LOG_ERROR, "failed to open battery full file: %s", battery_full_path);
        return 0;
    }

    unsigned int full = 0;
    if (fscanf(file, "%u", &full) != 1) {
        logger_log(LOG_ERROR, "failed to read battery full value");
        fclose(file);
        return 0;
    }

    fclose(file);
    return full;
}

static uint8_t get_battery_status(void)
{
    char status[32];
    FILE* file = fopen(battery_status_path, "r");
    if (!file) {
        logger_log(LOG_ERROR, "failed to open battery status file: %s", battery_status_path);
        return BATTERY_STATUS_UNKNOWN;
    }

    if (fgets(status, 32, file) == NULL) {
        logger_log(LOG_ERROR, "failed to read battery status");
        fclose(file);
        return BATTERY_STATUS_UNKNOWN;
    }

    // remove newline character
    status[strcspn(status, "\n")] = 0;
    fclose(file);

    if (strcmp(status, "Charging") == 0) {
        return BATTERY_STATUS_CHARGING;
    } else if (strcmp(status, "Discharging") == 0) {
        return BATTERY_STATUS_DISCHARGING;
    } else if (strcmp(status, "Not charging") == 0) {
        return BATTERY_STATUS_NOT_CHARGING;
    } else if (strcmp(status, "Full") == 0) {
        return BATTERY_STATUS_FULL;
    }

    return BATTERY_STATUS_UNKNOWN;
}

unsigned int get_battery_now(void)
{
    int now = 0;
    FILE* file = fopen(battery_now_path, "r");

    if (!file) {
        logger_log(LOG_ERROR, "failed to open battery now file: %s", battery_now_path);
        return 0;
    }

    if (fscanf(file, "%d", &now) != 1) {
        logger_log(LOG_ERROR, "failed to read battery now value");
        fclose(file);
        return 0;
    }

    fclose(file);
    return now;
}

char* get_battery_icon_from_percentage(float percentage)
{
    if (percentage >= 100.0f) {
        return "󰁹";
    } else if (percentage >= 90.0f) {
        return "󰂂";
    } else if (percentage >= 80.0f) {
        return "󰂁";
    } else if (percentage >= 70.0f) {
        return "󰂀";
    } else if (percentage >= 60.0f) {
        return "󰁿";
    } else if (percentage >= 50.0f) {
        return "󰁾";
    } else if (percentage >= 40.0f) {
        return "󰁽";
    } else if (percentage >= 30.0f) {
        return "󰁼";
    } else if (percentage >= 20.0f) {
        return "󰁻";
    } else {
        return "󰁺";
    }
}

char* battery_icon(float percentage)
{
    BatteryStatus status = get_battery_status();
    switch (status) {
    case BATTERY_STATUS_CHARGING:
        return "󰂄";
    case BATTERY_STATUS_FULL:
        return "󰁹";
    case BATTERY_STATUS_DISCHARGING:
    case BATTERY_STATUS_NOT_CHARGING:
        return get_battery_icon_from_percentage(percentage);
    default:
        return "󰂃";
    }
}

char* get_battery_color(float percentage)
{
    if (percentage >= 75.0f) {
        return GREEN;
    } else if (percentage >= 50.0f) {
        return YELLOW;
    } else if (percentage >= 25.0f) {
        return MAGENTA;
    } else {
        return RED;
    }
}

void* module_init(void* _state)
{
    ModuleState* state = _state;
    background = get_bg_color(state->config_get(state->name, "background"), "default");
    char buffer[32];
    uint16_t full = 0;
    uint16_t now = 0;
    char* config_battery_name = state->config_get(state->name, "source");

    // initialize
    init_battery_paths(config_battery_name);
    full = get_battery_full() / 1000;

    while (*state->running) {
        now = get_battery_now() / 1000;
        float percentage = ((float)now / full) * 100;
        snprintf(buffer, 32, "%s%s%s %.0f%%",
            get_battery_color(percentage), background, battery_icon(percentage), percentage);

        string_set_cstr(state->data, buffer);
        state->signal_render();
        msleep(60000);
    }

    return NULL;
}
