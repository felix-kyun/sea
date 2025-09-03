#include "log.h"
#include "colors.h"
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

#define TIMESTAMP_BUFFER_SIZE 32
#define TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"

// singleton logger instance
Logger* logger = NULL;
char timestamp_buffer[TIMESTAMP_BUFFER_SIZE];

static const char* level_to_string(enum LogLevel level)
{
    switch (level) {
    case LOG_DEBUG:
        return "DEBUG";
    case LOG_SUCCESS:
        return "SUCCESS";
    case LOG_INFO:
        return "INFO";
    case LOG_WARN:
        return "WARN";
    case LOG_ERROR:
        return "ERROR";
    case LOG_FATAL:
        return "FATAL";
    default:
        return "UNKNOWN";
    }
}

static void get_timestamp(void)
{
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(timestamp_buffer, TIMESTAMP_BUFFER_SIZE, TIMESTAMP_FORMAT, t);
}

void logger_init(const char* filename, bool stdout_enabled)
{
    logger = (Logger*)malloc(sizeof(Logger));
    logger->stdout_enabled = stdout_enabled;
    logger->file = fopen(filename, "a");
}

void logger_free(void)
{
    if (logger->file) {
        fclose(logger->file);
    }
    free(logger);
}

void logger_log(enum LogLevel level, const char* message, ...)
{
    if (!logger) {
        return;
    }

    va_list args;
    va_start(args, message);

    get_timestamp();
    if (logger->stdout_enabled) {
        printf("%s | %s\t | ", timestamp_buffer, level_to_string(level));
        vprintf(message, args);
        printf("\n");
    }

    if (logger->file) {
        fprintf(logger->file, "%s | %s\t | ", timestamp_buffer, level_to_string(level));
        vfprintf(logger->file, message, args);
        fprintf(logger->file, "\n");
        fflush(logger->file);
    }

    va_end(args);
}
