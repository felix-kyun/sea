#pragma once
#include <stdbool.h>

typedef struct Option {
    char short_name;
    const char* long_name;
    const char* description;
    const char* value;
    bool is_set;
    bool has_value;
    struct Option* next;
} Option;

typedef struct Argument {
    const char* name;
    const char* description;
    const char* value;
    bool is_required;
    struct Argument* next;
} Argument;

// creates an option
// options are prefixed with - or --
void set_option(
    const char short_name,
    const char* long_name,
    const char* description,
    bool has_value);

// creates an argument
// arguments are positional and have no prefix
void set_argument(
    const char* name,
    const char* description,
    bool is_required);

// parse command line arguments
void parse_args(int argc, char** argv);

// searching
// returns NULL if not found
Option* get_long_option(const char* long_name);
Option* get_short_option(const char short_name);
Argument* get_argument(const char* name);

// free all allocated internal memory
void free_args(void);
