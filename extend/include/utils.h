#pragma once
#include <glib.h>
#include <spa/buffer/buffer.h>

#define f_tag() g_debug("%s", __FUNCTION__)

static inline void
debug_spa_data(struct spa_data *data)
{
    g_debug("spa_data:");
    g_debug("maxsize: %u", data->maxsize);
    g_debug("chunk_size: %u", data->chunk->size);
    g_debug("chunk_stride: %i", data->chunk->stride);
}
