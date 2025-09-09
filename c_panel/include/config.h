#pragma once
#include "_config.macros.h"
#include <stdbool.h>

typedef struct Config {
    const char* log_file;
    bool log_to_stdout;
} Config;

extern Config config;

void config_init(int argc, char** argv);
void config_free(void);
