#define _GNU_SOURCE
#include "colors.h"
#include "modules/modules.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* color = "";
#define UPDATE_ICON "󰏔 "

static inline void
set_updates(ModuleState* state, const char* count_string)
{
    static char buffer[32];
    snprintf(buffer, 32, "%s" UPDATE_ICON "%s", color, count_string);

    if (!string_equals_cstr(state->data, buffer)) {
        string_set_cstr(state->data, buffer);
        state->signal_render();
    }
}

char*
get_updates(void)
{
    char  buffer[32];
    FILE* fp = popen("checkupdates | wc -l", "r");

    fgets(buffer, 32, fp);
    buffer[strcspn(buffer, "\n")] = 0;

    pclose(fp);
    return strdup(buffer);
}

static void
module_on_click(ModuleState* state)
{
    (void)state;

    char* updates = get_updates();
    if (strcmp(updates, "0") == 0) {
        free(updates);
        return;
    }
    free(updates);

    system("kitty -e sh -c '"
           "sudo pacman -Syu;"
           "read -n1 -srp \"Press any key to exit...\""
           "'");
}

void*
module_init(void* _state)
{
    ModuleState* state   = _state;
    color                = get_module_fg_color(state, "magenta");
    state->on_left_click = module_on_click;

    char* binary_path = find_binary("checkupdates");
    if (!binary_path) {
        set_updates(state, "error");
        free(binary_path);
        return NULL;
    }
    free(binary_path);

    set_updates(state, "0");
    while (*state->running) {
        char* updates = get_updates();
        set_updates(state, updates);
        free(updates);
        msleep(60 * 60 * 1000);
    }

    return NULL;
}
