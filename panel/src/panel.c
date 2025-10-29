#include "panel.h"
#include "config/config.h"
#include "log.h"
#include "modules/modules.h"
#include "render.h"
#include <dlfcn.h>
#include <stdlib.h>

// global
bool running;
ModuleState* module_states;
void** module_handles;
pthread_t* module_threads;
RenderSignal render_signal
    = {
          .mutex = PTHREAD_MUTEX_INITIALIZER,
          .cond = PTHREAD_COND_INITIALIZER,
      };

void panel_init(void)
{
    // set panel state to running
    running = true;

    // load renderer specific stuff
    render_init();

    // create plugin states
    module_states
        = malloc(sizeof(ModuleState) * MODULE_COUNT);

    module_handles = malloc(sizeof(void*) * MODULE_COUNT);

    module_threads = malloc(sizeof(pthread_t) * MODULE_COUNT);

    for (int i = 0; i < MODULE_COUNT; i++) {
        module_handles[i] = NULL;
        module_threads[i] = 0;

        module_states[i].running = &running;
        module_states[i].signal_render = panel_signal_render;

        module_states[i].data = string_new(" ");
        module_states[i].cleanup = NULL;

        // event handlers
        module_states[i].on_left_click = NULL;
        module_states[i].on_right_click = NULL;
        module_states[i].on_middle_click = NULL;
        module_states[i].on_scroll_up = NULL;
        module_states[i].on_scroll_down = NULL;
    }
}

void panel_signal_render(void)
{
    pthread_mutex_lock(&render_signal.mutex);
    pthread_cond_broadcast(&render_signal.cond);
    pthread_mutex_unlock(&render_signal.mutex);
}

typedef void* (*start_routine_t)(void*);
void spawn_module(const char* module_name, int index)
{
    logger_log(LOG_INFO, "spawning module: %s", module_name);

    char module_path[PATH_MAX];
    snprintf(module_path, PATH_MAX, "%s/modules/%s.so", config.current_path, module_name);

    module_handles[index] = dlopen(module_path, RTLD_NOW);
    if (!module_handles[index]) {
        logger_log(LOG_ERROR, "failed to load module %s: %s", module_name, dlerror());
        return;
    }

    start_routine_t init_func = (start_routine_t)dlsym(module_handles[index], "module_init");
    if (!init_func) {
        logger_log(LOG_ERROR, "failed to find module_init in %s: %s", module_name, dlerror());
        dlclose(module_handles[index]);
        return;
    }

    if (pthread_create(module_threads + index, NULL, init_func, module_states + index) != 0) {
        logger_log(LOG_ERROR, "failed to create plugin thread");
    }
}

void panel_init_modules(void)
{
    logger_log(LOG_INFO, "initializing plugins");
    logger_log(LOG_INFO, "Found %d plugins", MODULE_COUNT);

    for (int i = 0; i < LEFT_COUNT; i++) {
        spawn_module(config.left_modules->items[i], i);
    }

    for (int i = 0; i < CENTER_COUNT; i++) {
        spawn_module(config.center_modules->items[i], i + LEFT_COUNT);
    }

    for (int i = 0; i < RIGHT_COUNT; i++) {
        spawn_module(config.right_modules->items[i], LEFT_COUNT + CENTER_COUNT + i);
    }

    logger_log(LOG_SUCCESS, "all plugins started");
}

void panel_free(void)
{
    pthread_mutex_destroy(&render_signal.mutex);
    pthread_cond_destroy(&render_signal.cond);

    // free plugin states
    for (int i = 0; i < MODULE_COUNT; i++) {
        pthread_cancel(module_threads[i]);
        pthread_join(module_threads[i], NULL);

        string_free(module_states[i].data);

        // call cleanup if registered
        if (module_states[i].cleanup) {
            module_states[i].cleanup();
        }
        // close module handle
        if (module_handles[i]) {
            dlclose(module_handles[i]);
        }
    }

    free(module_states);
    free(module_handles);
    // show cursor
    printf("\033[?25h");
}

void panel_stop(void)
{
    running = false;
    panel_signal_render();
}
