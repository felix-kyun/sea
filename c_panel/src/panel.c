#include "panel.h"
#include "log.h"
#include <stdlib.h>

RenderSignal render_signal = {
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .cond = PTHREAD_COND_INITIALIZER,
};

PluginState* plugin_states;

void panel_spawn_plugin_thread(void* (*start_routine)(void*), PluginContext* context)
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

void panel_init_plugins(StartRoutine* enabled_plugins, size_t plugin_count)
{
    plugin_states = malloc(sizeof(PluginState) * plugin_count);

    // spawn plugin
    for (size_t i = 0; i < plugin_count; i++) {
        // initialize the plugin context
        PluginContext* context = malloc(sizeof(PluginContext));
        context->state = &plugin_states[i];
        context->state->data = string_new("");
        context->render_signal = &render_signal;

        panel_spawn_plugin_thread(enabled_plugins[i], context);
    }
}

void panel_render(void)
{
}

void panel_free(void)
{
    pthread_mutex_destroy(&render_signal.mutex);
    pthread_cond_destroy(&render_signal.cond);
}
