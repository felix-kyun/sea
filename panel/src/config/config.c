#define _GNU_SOURCE

#include "config/config.h"
#include "hashmap.h"
#include "hashmap_string.h"
#include "log.h"
#include <libgen.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

hashmap_t* map;

static void
hashmap_as_value_free(void* value)
{
    hashmap_destroy(*(hashmap_t**)value);
    free(value);
}

void
config_init(void)
{
    // string -> hashmap_t* (void*)
    map = hashmap_create(sizeof(char*), sizeof(void*), 0, 0.75, fnv1a_hash_string, hashmap_cmp_string,
        hashmap_key_copy_string, NULL, hashmap_key_free_string, hashmap_as_value_free);

    char    exec_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exec_path, PATH_MAX - 1);

    if (len != -1) {
        exec_path[len] = '\0';
        strncpy(config.current_path, dirname(exec_path), PATH_MAX);
    } else {
        logger_log(LOG_ERROR, "Failed to get executable path");
        exit(1);
    }
}

void
config_parse(const char* filepath)
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
        sscanf(line, " %63s = %*['\"]%959[^'\"\n]", key, value);

        hashmap_t* section_map = hashmap_get(map, section);
        if (!section_map) {
            section_map = hashmap_create(sizeof(char*), sizeof(void*), 0, 0.75, fnv1a_hash_string, hashmap_cmp_string,
                hashmap_key_copy_string, hashmap_value_copy_string, hashmap_key_free_string, hashmap_value_free_string);
            hashmap_set(map, section, &section_map);
        } else {
            section_map = *(hashmap_t**)section_map;
        }
        hashmap_set(section_map, key, value);
    }

    fclose(file);
}
char*
config_get(char* section, char* key)
{
    hashmap_t* section_map = hashmap_get(map, section);
    if (!section_map) {
        return NULL;
    }
    return (char*)hashmap_get(*(hashmap_t**)section_map, key);
}

void
config_free(void)
{
    hashmap_destroy(map);
}
