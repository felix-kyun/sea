#include "colors.h"
#include "modules/modules.h"
#include <string.h>

typedef struct Color {
    const char* name;
    const char* code;
} Color;

static Color fg_colors[] = {
    { "black", BLACK },
    { "red", RED },
    { "green", GREEN },
    { "yellow", YELLOW },
    { "blue", BLUE },
    { "magenta", MAGENTA },
    { "cyan", CYAN },
    { "white", WHITE },
    { "bold_black", BOLD_BLACK },
    { "bold_red", BOLD_RED },
    { "bold_green", BOLD_GREEN },
    { "bold_yellow", BOLD_YELLOW },
    { "bold_blue", BOLD_BLUE },
    { "bold_magenta", BOLD_MAGENTA },
    { "bold_cyan", BOLD_CYAN },
    { "bold_white", BOLD_WHITE },
    { "underline_black", UNDERLINE_BLACK },
    { "underline_red", UNDERLINE_RED },
    { "underline_green", UNDERLINE_GREEN },
    { "underline_yellow", UNDERLINE_YELLOW },
    { "underline_blue", UNDERLINE_BLUE },
    { "underline_magenta", UNDERLINE_MAGENTA },
    { "underline_cyan", UNDERLINE_CYAN },
    { "underline_white", UNDERLINE_WHITE },
};

static Color bg_colors[] = {
    { "black", BG_BLACK },
    { "red", BG_RED },
    { "green", BG_GREEN },
    { "yellow", BG_YELLOW },
    { "blue", BG_BLUE },
    { "magenta", BG_MAGENTA },
    { "cyan", BG_CYAN },
    { "white", BG_WHITE },
    { "default", BG_DEFAULT },
};

static char* get_fg_color(const char* color_name, const char* default_color)
{
    const char* name = color_name ? color_name : default_color;

    for (size_t i = 0; i < sizeof(fg_colors) / sizeof(Color); i++) {
        if (strcmp(fg_colors[i].name, name) == 0) {
            return (char*)fg_colors[i].code;
        }
    }
    return RESET;
}

static char* get_bg_color(const char* color_name, const char* default_color)
{
    const char* name = color_name ? color_name : default_color;

    for (size_t i = 0; i < sizeof(bg_colors) / sizeof(Color); i++) {
        if (strcmp(bg_colors[i].name, name) == 0) {
            return (char*)bg_colors[i].code;
        }
    }
    return RESET;
}

char* get_module_fg_color(const ModuleState* state, const char* default_color)
{
    return get_fg_color(state->config_get(state->name, "color"), default_color);
}

char* get_module_bg_color(const ModuleState* state)
{
    return get_bg_color(state->config_get(state->name, "background"), "default");
}
