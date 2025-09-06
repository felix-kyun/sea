#include "panel.h"
#include "config.h"
#include "log.h"
#include "plugins/plugins.h"
#include "render.h"
#include <stdlib.h>

// global
bool running;
PluginState* plugin_states;
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
    plugin_states = malloc(sizeof(PluginState) * PLUGIN_COUNT);

    for (int i = 0; i < PLUGIN_COUNT; i++) {
        plugin_states[i].data = string_new(" ");
        plugin_states[i].cleanup = NULL;
    }
}

void panel_spawn_plugin_thread(void* (*start_routine)(void*), PluginState* context)
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
    panel_spawn_plugin_thread(plugin_##name, &plugin_states[name##_idx]); \
    logger_log(LOG_SUCCESS, "started plugin: " #name);
void panel_init_plugins(void)
{
    logger_log(LOG_INFO, "initializing plugins");
    logger_log(LOG_INFO, "Found %d plugins", PLUGIN_COUNT);
    logger_log(LOG_INFO, "Left: %d", PLUGIN_LEFT_COUNT);
    logger_log(LOG_INFO, "Center: %d", PLUGIN_CENTER_COUNT);
    logger_log(LOG_INFO, "Right: %d", PLUGIN_RIGHT_COUNT);

    // spawn enabled plugins
    PLUGIN_LIST

    logger_log(LOG_SUCCESS, "all plugins started");
}
#undef X

void panel_free(void)
{
    pthread_mutex_destroy(&render_signal.mutex);
    pthread_cond_destroy(&render_signal.cond);

    // free plugin states
    for (int i = 0; i < PLUGIN_COUNT; i++) {
        string_free(plugin_states[i].data);

        // call cleanup if registered
        if (plugin_states[i].cleanup) {
            plugin_states[i].cleanup();
        }
    }

    free(plugin_states);
    // show cursor
    printf("\033[?25h");
}

void panel_stop(void)
{
    running = false;
    panel_signal_render();
}
