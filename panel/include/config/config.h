#pragma once
#include "config/utils.h"
#include <linux/limits.h>
#include <stdbool.h>

#define CONFIG_FILE "/home/felix/.config/sea/panel.ini"
#define LINE_MAX_LENGTH 1024
#define SECTION_MAX_LENGTH 64
#define KEY_MAX_LENGTH 64
#define VALUE_MAX_LENGTH (LINE_MAX_LENGTH - SECTION_MAX_LENGTH)

#define LEFT_COUNT (config.left_modules->length)
#define CENTER_COUNT (config.center_modules->length)
#define RIGHT_COUNT (config.right_modules->length)
#define MODULE_COUNT (LEFT_COUNT + CENTER_COUNT + RIGHT_COUNT)

typedef struct Config {
    const char* log_file;
    const char* config_file;
    bool log_to_stdout;
    char current_path[PATH_MAX];
    int module_spacing;
    int panel_padding;
    bool auto_reset_style;
    StringArray* left_modules;
    StringArray* center_modules;
    StringArray* right_modules;
} Config;

extern Config config;

void create_app_config(int argc, char** argv);
void destroy_app_config(void);

void config_init(void);
void config_parse(const char* filepath);
char* config_get(const char* section, const char* key);
void config_free(void);
