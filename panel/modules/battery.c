#include "colors.h"
#include "log.h"
#include "modules/modules.h"
#include "state.h"
#include "string-utf8.h"
#include "utils.h"
#include <dirent.h>
#include <linux/limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define BATTERY_FILES_PATH "/sys/class/power_supply/"

static char battery_path[PATH_MAX] = { 0 };
static char battery_full_path[PATH_MAX] = { 0 };
static char battery_now_path[PATH_MAX] = { 0 };
static char battery_status_path[PATH_MAX] = { 0 };

typedef enum BatteryStatus {
    BATTERY_STATUS_UNKNOWN = 0,
    BATTERY_STATUS_CHARGING,
    BATTERY_STATUS_DISCHARGING,
    BATTERY_STATUS_NOT_CHARGING,
    BATTERY_STATUS_FULL,
} BatteryStatus;

static char* get_battery_file(void)
{
    struct dirent* entry;
    DIR* dp = opendir(BATTERY_FILES_PATH);

    if (dp == NULL) {
        logger_log(LOG_ERROR, "failed to open directory: " BATTERY_FILES_PATH);
        return NULL;
    }

    while ((entry = readdir(dp))) {
        if (strncmp(entry->d_name, "BAT", 3) == 0) {
            snprintf(battery_path, PATH_MAX, "%s%s", BATTERY_FILES_PATH, entry->d_name);
            closedir(dp);
            return battery_path;
        }
    }

    closedir(dp);
    return NULL;
}

static void init_battery_paths(void)
{
    char* battery = get_battery_file();
    if (battery == NULL) {
        logger_log(LOG_ERROR, "no battery found");
        return;
    }

    logger_log(LOG_INFO, "found battery: %s", battery);

    snprintf(battery_full_path, PATH_MAX, "%s/energy_full", battery);
    snprintf(battery_now_path, PATH_MAX, "%s/energy_now", battery);
    snprintf(battery_status_path, PATH_MAX, "%s/status", battery);

    logger_log(LOG_INFO, "battery paths initialized:");
    logger_log(LOG_INFO, "full: %s", battery_full_path);
    logger_log(LOG_INFO, "now: %s", battery_now_path);
    logger_log(LOG_INFO, "status: %s", battery_status_path);
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

char* battery_icon(void)
{
    BatteryStatus status = get_battery_status();
    switch (status) {
    case BATTERY_STATUS_CHARGING:
        return "󰂄";
    case BATTERY_STATUS_DISCHARGING:
        return "󰁺";
    case BATTERY_STATUS_NOT_CHARGING:
        return "󰂂";
    case BATTERY_STATUS_FULL:
        return "󰁹";
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
    char buffer[32];
    uint16_t full = 0;
    uint16_t now = 0;

    // initialize
    init_battery_paths();
    full = get_battery_full() / 1000;

    while (running) {
        now = get_battery_now() / 1000;
        float percentage = ((float)now / full) * 100;
        snprintf(buffer, 32, "%s%s %.0f%% " RESET,
            get_battery_color(percentage), battery_icon(), percentage);

        string_set_cstr(state->data, buffer);
        panel_signal_render();
        msleep(60000);
    }

    return NULL;
}
