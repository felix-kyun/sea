#pragma once
#include "modules/modules.h"
#include <pthread.h>

typedef struct RenderSignal {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} RenderSignal;

// singleton render signal instance to be shared between plugin threads
extern RenderSignal render_signal;

// array of module states
typedef void* (*StartRoutine)(void*);

// to store module states
extern ModuleState* module_states;

// methods
void panel_spawn_module_thread(void* (*start_routine)(void*), ModuleState* context);
void panel_init(void);
void panel_render(void);
void panel_init_modules(void);
void panel_free(void);
void panel_signal_render(void);
