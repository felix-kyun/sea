#define _GNU_SOURCE
#include "config/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

StringArray* create_string_array(void)
{
    StringArray* array = (StringArray*)malloc(sizeof(StringArray));

    array->length = 0;
    array->items = NULL;

    return array;
}

void string_array_add(StringArray* array, const char* item)
{
    array->items = (char**)realloc(array->items, sizeof(char*) * (array->length + 1));
    array->items[array->length] = strdup(item);
    array->length++;
}

void free_string_array(StringArray* array)
{
    free(array->items);
    free(array);
}

LinearMap* create_map(void)
{
    LinearMap* new_map = (LinearMap*)malloc(sizeof(LinearMap));

    new_map->length = 0;
    new_map->keys = create_string_array();
    new_map->values = create_string_array();

    return new_map;
}

void map_set(LinearMap* map, const char* key, const char* value)
{
    for (int i = 0; i < map->length; i++) {
        if (strcmp(map->keys->items[i], key) == 0) {
            map->values->items[i] = (char*)value;
            return;
        }
    }

    string_array_add(map->keys, key);
    string_array_add(map->values, value);
    map->length++;
}

const char* map_get(LinearMap* map, const char* key)
{
    for (int i = 0; i < map->length; i++) {
        if (strcmp(map->keys->items[i], key) == 0) {
            return map->values->items[i];
        }
    }

    return NULL;
}

void free_map(LinearMap* map)
{
    free_string_array(map->keys);
    free_string_array(map->values);
    free(map);
}

bool starts_with(const char* str, const char* prefix)
{
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

char* create_key(const char* section, const char* key)
{
    size_t len = strlen(section) + strlen(key) + 2;
    char* full_key = (char*)malloc(len);

    snprintf(full_key, len, "%s.%s", section, key);

    return full_key;
}
