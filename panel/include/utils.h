#pragma once
#include "config/utils.h"
#include <stdbool.h>
#include <stddef.h>

void msleep(long msec);
void padding(int count);
void string_limit(char* str, size_t max_chars);
void debug_config(void);
bool includes(const char** array, int len, const char* item);
char* get_color(const char* color_name);
