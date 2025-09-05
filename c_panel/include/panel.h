#pragma once
#include "plugins/plugins.h"
#include <pthread.h>

typedef struct RenderSignal {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} RenderSignal;

// singleton render signal instance to be shared between plugin threads
extern RenderSignal render_signal;

// array of plugin states
typedef void* (*StartRoutine)(void*);

// to store plugin states
extern PluginState* plugin_states;

// methods
void panel_spawn_plugin_thread(void* (*start_routine)(void*), PluginState* context);
void panel_init(void);
void panel_render(void);
void panel_init_plugins(void);
void panel_free(void);
