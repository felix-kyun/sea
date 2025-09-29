#include "panel.h"
#include "config.h"
#include "log.h"
#include "modules/modules.h"
#include "render.h"
#include <stdlib.h>

// global
bool running;
ModuleState* module_states;
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
    module_states = malloc(sizeof(ModuleState) * MODULE_COUNT);

    for (int i = 0; i < MODULE_COUNT; i++) {
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

void panel_spawn_module_thread(void* (*start_routine)(void*), ModuleState* context)
{
    pthread_t thread;
    if (pthread_create(&thread, NULL, start_routine, context) != 0) {
        logger_log(LOG_ERROR, "failed to create plugin thread");
    }
    pthread_detach(thread);
}

void panel_signal_render(void)
{
    pthread_mutex_lock(&render_signal.mutex);
    pthread_cond_broadcast(&render_signal.cond);
    pthread_mutex_unlock(&render_signal.mutex);
}

#define X(name)                                                           \
    panel_spawn_module_thread(module_##name, &module_states[name##_idx]); \
    logger_log(LOG_SUCCESS, "started plugin: " #name);
void panel_init_modules(void)
{
    logger_log(LOG_INFO, "initializing plugins");
    logger_log(LOG_INFO, "Found %d plugins", MODULE_COUNT);
    logger_log(LOG_INFO, "Left: %d", LEFT_COUNT);
    logger_log(LOG_INFO, "Center: %d", CENTER_COUNT);
    logger_log(LOG_INFO, "Right: %d", RIGHT_COUNT);

    // spawn enabled plugins
    MODULE_LIST

    logger_log(LOG_SUCCESS, "all plugins started");
}
#undef X

void panel_free(void)
{
    pthread_mutex_destroy(&render_signal.mutex);
    pthread_cond_destroy(&render_signal.cond);

    // free plugin states
    for (int i = 0; i < MODULE_COUNT; i++) {
        string_free(module_states[i].data);

        // call cleanup if registered
        if (module_states[i].cleanup) {
            module_states[i].cleanup();
        }
    }

    free(module_states);
    // show cursor
    printf("\033[?25h");
}

void panel_stop(void)
{
    running = false;
    panel_signal_render();
}
