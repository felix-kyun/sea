#define _GNU_SOURCE

#include "modules/modules.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void set_updates(ModuleState* state, const char* count_string)
{
    static char buffer[32];
    snprintf(buffer, 32, "󰏔 %s", count_string);

    if (!string_equals_cstr(state->data, buffer)) {
        string_set_cstr(state->data, buffer);
        state->signal_render();
    }
}

static void module_on_click(ModuleState* state)
{
    (void)state;
    system("kitty -e sh -c '"
           "sudo pacman -Syu;"
           "read -n1 -srp \"Press any key to exit...\""
           "'");
}

void* module_init(void* _state)
{
    ModuleState* state = _state;
    state->on_left_click = module_on_click;

    char* binary_path = find_binary("checkupdates");
    if (!binary_path) {
        set_updates(state, "error");
        free(binary_path);
        return NULL;
    }
    free(binary_path);

    char buffer[32];
    while (*state->running) {
        FILE* fp = popen("checkupdates | wc -l", "r");

        fgets(buffer, 32, fp);
        buffer[strcspn(buffer, "\n")] = 0;
        set_updates(state, buffer);

        pclose(fp);
        msleep(60 * 60 * 1000);
    }

    return NULL;
}
