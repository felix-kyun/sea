#define _GNU_SOURCE
#include "config/config.h"
#include "config/utils.h"
#include <libgen.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

LinearMap* map = NULL;

void config_init(void)
{
    // get exec path
    char exec_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exec_path, PATH_MAX - 1);
    if (len != -1) {
        exec_path[len] = '\0';
        strncpy(config.current_path, dirname(exec_path), PATH_MAX);
    } else {
        perror("Failed to get executable path");
        strncpy(config.current_path, "", PATH_MAX);
    }

    map = create_map();
}

void config_parse(const char* filepath)
{
    FILE* file = fopen(filepath, "r");

    if (!file) {
        perror("Failed to open config file");
        return;
    }

    char line[LINE_MAX_LENGTH];
    char section[SECTION_MAX_LENGTH] = "";
    char key[KEY_MAX_LENGTH];
    char value[VALUE_MAX_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        // ignore comments and empty lines
        if (starts_with(line, ";") || line[0] == '\n')
            continue;

        //  parse section headers
        if (starts_with(line, "[")) {
            sscanf(line, "[%63[^]]", section);
            continue;
        }

        // parse kv pairs
        sscanf(line, " %63[^= ]%*[ ]=%*[ ]%959[^\n]", key, value);
        char* full_key = create_key(section, key);

        map_set(map, full_key, value);
        free(full_key);
    }

    fclose(file);
}

char* config_get(const char* key)
{
    return (char*)map_get(map, key);
}

void config_free(void)
{
    free_map(map);
}
