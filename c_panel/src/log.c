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
        return "D";
    case LOG_SUCCESS:
        return "S";
    case LOG_INFO:
        return "I";
    case LOG_WARN:
        return "W";
    case LOG_ERROR:
        return "E";
    default:
        return "?";
    }
}

static const char* level_to_color(enum LogLevel level)
{
    switch (level) {
    case LOG_DEBUG:
        return WHITE;
    case LOG_SUCCESS:
        return GREEN;
    case LOG_INFO:
        return BLUE;
    case LOG_WARN:
        return YELLOW;
    case LOG_ERROR:
        return BOLD_RED;
    default:
        return WHITE;
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
    logger->file = fopen(filename, "w");
}

void logger_free(void)
{
    // reset terminal colors
    printf(RESET);
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
    printf("%s", level_to_color(level));
    if (logger->stdout_enabled) {
        va_list args_copy;
        va_copy(args_copy, args);
        printf("%s | %s | ", timestamp_buffer, level_to_string(level));
        vprintf(message, args_copy);
        printf(RESET "\n");
        va_end(args_copy);
    }

    if (logger->file) {
        fprintf(logger->file, "%s | %s | ", timestamp_buffer, level_to_string(level));
        vfprintf(logger->file, message, args);
        fprintf(logger->file, "\n");
        fflush(logger->file);
    }

    va_end(args);
}
