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
    DEBUG("log file: %s", config.log_file);
    DEBUG("log to stdout: %d", config.log_to_stdout);
    DEBUG("------------------");
}
