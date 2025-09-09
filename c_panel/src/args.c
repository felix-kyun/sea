#include "args.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Option* options = NULL;
Argument* args = NULL;

static inline void throw_error(const char* message, const char* arg)
{
    fprintf(stderr, "%s: %s\n", message, arg);
    raise(SIGINT);
}

static inline bool startsWith(const char* str, const char* prefix)
{
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

static Option* search_option(const char* name)
{
    Option* current = options;
    while (current) {
        if ((current->short_name && current->short_name == name[0]) || (current->long_name && strcmp(current->long_name, name) == 0)) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void free_args(void)
{
    Option* opt = options;
    Argument* arg = args;

    while (opt) {
        Option* next = opt->next;
        free(opt);
        opt = next;
    }

    while (arg) {
        Argument* next = arg->next;
        free(arg);
        arg = next;
    }
}

void set_option(
    const char short_name,
    const char* long_name,
    const char* description,
    bool has_value)
{
    Option* opt = malloc(sizeof(Option));

    // initialize
    opt->short_name = short_name;
    opt->long_name = long_name;
    opt->description = description;
    opt->is_set = false;
    opt->value = NULL;
    opt->has_value = has_value;
    opt->next = NULL;

    // append to list
    if (!options)
        options = opt;
    else {
        opt->next = options;
        options = opt; // prependinga as order doesn't matter
    }
}

void set_argument(
    const char* name,
    const char* description,
    bool is_required)
{
    Argument* arg = malloc(sizeof(Argument));

    // initialize
    arg->name = name;
    arg->description = description;
    arg->value = NULL;
    arg->is_required = is_required;
    arg->next = NULL;

    // append to list
    if (!args)
        args = arg;
    else {
        Argument* current = args;
        while (current->next)
            current = current->next;
        current->next = arg; // preserving order
    }
}

void parse_args(int argc, char** argv)
{
    int idx = 1;
    while (idx < argc) {
        char* current = argv[idx];

        if (startsWith(current, "--")) { // long option
            Option* opt = search_option(current + 2);
            if (!opt)
                throw_error("unknown option", current);

            opt->is_set = true;
            idx++;

            if (opt->has_value) {
                if (idx >= argc)
                    throw_error("option requires a value", current);
                opt->value = argv[idx];
                idx++;
            }
        } else if (startsWith(current, "-")) { // short option
            Option* opt = search_option(current + 1);
            if (!opt)
                throw_error("unknown option", current);

            opt->is_set = true;
            bool combined = strlen(current) > 2;
            idx++;

            if (opt->has_value && combined)
                throw_error("cannot combine option with value", current);

            if (opt->has_value && !combined) {
                if (idx >= argc)
                    throw_error("option requires a value", current);
                opt->value = argv[idx];
                idx++;
            }

            if (combined) {
                // combined short options
                // example: -abc <some_optional_value>
                // means -a -b -c <some_optional_value>
                for (size_t i = 1; i < strlen(current); i++) {
                    char c[2] = { current[i], '\0' };
                    Option* sub_opt = search_option(c);

                    if (!sub_opt)
                        throw_error("unknown option", c);

                    if (sub_opt->has_value && i != strlen(current) - 1)
                        throw_error("cannot combine option with value", c);

                    sub_opt->is_set = true;
                    if (sub_opt->has_value) {
                        if (idx >= argc)
                            throw_error("option requires a value", c);
                        sub_opt->value = argv[idx];
                        idx++;
                    }
                }
            }
        } else {
            // argument
            Argument* arg = args;
            while (arg && arg->value)
                arg = arg->next;

            if (!arg)
                throw_error("too many arguments", current);

            arg->value = current;
            idx++;
        }
    }
}

Option* get_long_option(const char* long_name)
{
    return search_option(long_name);
}

Option* get_short_option(const char short_name)
{
    char name[2] = { short_name, '\0' };
    return search_option(name);
}

Argument* get_argument(const char* name)
{
    Argument* current = args;
    while (current) {
        if (strcmp(current->name, name) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}
