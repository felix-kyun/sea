#pragma once

#include "string-utf8.h"
#include <pthread.h>
#include <stdbool.h>

typedef struct Overlay {
    string* content;
    pthread_t cleaner;
    pthread_t watcher;
    void (*signal_render)(void);
    bool active;
    pthread_mutex_t lock;
    int fifo_fd;
} Overlay;

extern Overlay overlay;

void overlay_init(void);
void overlay_set(const char* content);
void* overlay_cleaner(void* _overlay);
void overlay_spawn_watcher(void);
void overlay_print(unsigned short terminal_width);

void overlay_free(void);
