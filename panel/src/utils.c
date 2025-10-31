#include "utils.h"
#include "config/config.h"
#include "log.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    DEBUG("module spacing: %d", config.module_spacing);
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

bool includes(const char** array, int len, const char* item)
{
    for (int i = 0; i < len; i++) {
        if (strcmp(array[i], item) == 0) {
            return true;
        }
    }
    return false;
}

char* create_padding_str(int count)
{
    if (count <= 0)
        return "";

    char* padding_string = (char*)malloc((count + 1) * sizeof(char));

    for (int i = 0; i < count; i++) {
        padding_string[i] = ' ';
    }
    padding_string[count] = '\0';

    return padding_string;
}
