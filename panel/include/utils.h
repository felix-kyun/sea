#pragma once
#include "config/utils.h"
#include <stdbool.h>
#include <stddef.h>

#define from_seconds(x) ((x) * 1000)
#define from_minutes(x) (from_seconds((x) * 60))
#define from_hours(x) (from_minutes((x) * 60))
#define from_days(x) (from_hours((x) * 24))

void msleep(long msec);

void padding(int count);

char* create_padding_str(int count);

void string_limit(char* str, size_t max_chars);

void debug_config(void);

bool includes(const char** array, int len, const char* item);

char* get_color(const char* color_name);

char* find_binary(const char* binary_name);

bool check_binaries(const char** binaries, size_t count);
