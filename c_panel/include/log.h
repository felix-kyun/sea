#pragma once

#include <stdbool.h>
#include <stdio.h>
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
void logger_log(enum LogLevel level, const char* message, ...);
void logger_free(void);
