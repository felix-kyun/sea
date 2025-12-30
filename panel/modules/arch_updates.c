#define _GNU_SOURCE

#include "modules/modules.h"
#include "utils.h"
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static inline char* find_binary(const char* binary_name)
{
    char* paths_system = getenv("PATH");
    if (paths_system == NULL) {
        return NULL;
    }

    char* paths = strdup(paths_system);

    char* saveptr;
    char* path = strtok_r(paths, ":", &saveptr);
    while (path != NULL) {
        char full_path[PATH_MAX];
        snprintf(full_path, PATH_MAX, "%s/%s", path, binary_name);
        if (access(full_path, X_OK) == 0) {
            free(paths);
            return strdup(full_path);
        }
        path = strtok_r(NULL, ":", &saveptr);
    }

    free(paths);
    return NULL;
}

static inline void set_updates(ModuleState* state, const char* count_string)
{
    static char buffer[32];
    snprintf(buffer, 32, "󰏔 %s", count_string);

    if (!string_equals_cstr(state->data, buffer)) {
        string_set_cstr(state->data, buffer);
        state->signal_render();
    }
}

void* module_init(void* _state)
{
    ModuleState* state = _state;

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
