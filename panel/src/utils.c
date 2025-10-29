#include "utils.h"
#include "config/config.h"
#include "log.h"
#include <stdio.h>
#include <time.h>

void msleep(long msec)
{
    struct timespec ts;
    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

void padding(int count)
{
    for (int i = 0; i < count; i++) {
        printf(" ");
    }
}

void string_limit(char* str, size_t max_chars)
{
    char* ptr = str;
    size_t idx = 0;

    while (*ptr && idx < max_chars) {
        idx++;
        ptr++;
    }

    *ptr = '\0';
}

void debug_config(void)
{
    DEBUG("----- CONFIG -----");
    DEBUG("current path: %s", config.current_path);
    DEBUG("log file: %s", config.log_file);
    DEBUG("log to stdout: %d", config.log_to_stdout);
    DEBUG("left modules:");
    for (int i = 0; i < config.left_modules->length; i++) {
        DEBUG("  - %s", config.left_modules->items[i]);
    }
    DEBUG("center modules:");
    for (int i = 0; i < config.center_modules->length; i++) {
        DEBUG("  - %s", config.center_modules->items[i]);
    }
    DEBUG("right modules:");
    for (int i = 0; i < config.right_modules->length; i++) {
        DEBUG("  - %s", config.right_modules->items[i]);
    }
    DEBUG("------------------");
}
