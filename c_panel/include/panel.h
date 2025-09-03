#pragma once
#include "plugins/plugins.h"
#include <pthread.h>

typedef struct RenderSignal {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} RenderSignal;

typedef struct PluginContext {
    PluginState* state;
    RenderSignal* render_signal;
} PluginContext;

// singleton render signal instance to be shared between plugin threads
extern RenderSignal render_signal;

// array of plugin states
typedef void* (*StartRoutine)(void*);

// methods
void panel_spawn_plugin_thread(void* (*start_routine)(void*), PluginContext* context);
void panel_signal_render(void);
void panel_render(void);
void panel_init_plugins(StartRoutine* enabled_plugins, size_t plugin_count);
void panel_free(void);
