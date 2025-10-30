#pragma once
#include <stdbool.h>

typedef struct StringArray {
    int length;
    char** items;
} StringArray;

StringArray* create_string_array(void);
void string_array_add(StringArray* array, const char* item);
void free_string_array(StringArray* array);

typedef struct LinearMap {
    int length;
    StringArray* keys;
    StringArray* values;
} LinearMap;

LinearMap* create_map(void);
void map_set(LinearMap* map, const char* section, const char* key, const char* value);
const char* map_get(LinearMap* map, const char* section, const char* key);
void free_map(LinearMap* map);

// utils
bool starts_with(const char* str, const char* prefix);
char* create_key(const char* section, const char* key);
void string_array_from_csv(StringArray* array, const char* csv);
