#pragma once
#include <stdbool.h>

#define CONFIG_FILE "/home/felix/.config/sea/panel.ini"
#define LINE_MAX_LENGTH 1024
#define SECTION_MAX_LENGTH 64
#define KEY_MAX_LENGTH 64
#define VALUE_MAX_LENGTH (LINE_MAX_LENGTH - SECTION_MAX_LENGTH)

typedef struct Config {
    const char* log_file;
    bool log_to_stdout;
} Config;

extern Config config;

void create_app_config(int argc, char** argv);

void config_init(void);
void config_parse(const char* filepath);
char* config_get(const char* key);
void config_free(void);
