#pragma once

#include <stdbool.h>
#include <stdio.h>

#ifndef NDEBUG
#define DEBUG(fmt, ...) logger_log(LOG_DEBUG, fmt, ##__VA_ARGS__)
#else
#define DEBUG(fmt, ...) \
    do {                \
    } while (0)
#endif

enum LogLevel {
    LOG_DEBUG = 0,
    LOG_SUCCESS,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
};

struct Logger {
    bool stdout_enabled;
    FILE* file;
};

typedef enum LogLevel LogLevel;
typedef struct Logger Logger;

// singleton logger instance
extern Logger* logger;

void logger_init(const char* filename, bool stdout_enabled);
void logger_log(enum LogLevel level, const char* message, ...) __attribute__((format(printf, 2, 3)));
void logger_free(void);
