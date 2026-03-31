#include "pipewire.h"
#include <glib-unix.h>
#include <glib.h>
#include <pipewire/context.h>
#include <pipewire/core.h>
#include <pipewire/loop.h>
#include <spa/pod/builder.h>
#include <stddef.h>

// TODO: reduce tight coupling

typedef struct _pw_data {
    // acquired from portal
    guint32 node_id;
    gint    source_fd;

    // pw
    struct pw_loop    *loop;
    struct pw_stream  *stream;
    struct pw_context *context;
    struct pw_core    *core;
    int                pw_fd;

    // glib
    GSource *source;
} pw_data;

static pw_data      data         = { 0 };
static GSourceFuncs source_funcs = {
    .dispatch = pw_dispatch_handler,
};

struct pw_loop *
pw_create_loop()
{
    data.loop = pw_loop_new(nullptr);
    return data.loop;
}

void
pw_setup(guint32 node, gint fd)
{
    g_debug("pw_setup invoked");

    data.node_id   = node;
    data.source_fd = fd;
    data.context   = pw_context_new(data.loop, nullptr, 0);
    data.core      = pw_context_connect_fd(data.context, data.source_fd, nullptr, 0);
    data.pw_fd     = pw_loop_get_fd(data.loop);

    // glib source
    data.source = g_source_new(&source_funcs, sizeof(GSource));
    g_source_add_unix_fd(data.source, data.pw_fd, G_IO_IN | G_IO_ERR);
    g_source_attach(data.source, nullptr);
    g_source_unref(data.source);
}

void
pw_free()
{
    pw_core_disconnect(data.core);
    pw_context_destroy(data.context);
    pw_loop_destroy(data.loop);
}

gboolean
pw_dispatch_handler(
    [[maybe_unused]] GSource *source, [[maybe_unused]] GSourceFunc callback, [[maybe_unused]] gpointer user_data)
{
    g_debug("pw_dispatch_handler invoked");
    int result = pw_loop_iterate(data.loop, 0);
    if (result < 0) {
        g_debug("pw_dispatch_handler: pw_loop_iterate failed: %d", result);
        return false;
    }
    return true;
}
