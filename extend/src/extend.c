#include "dbus.h"
#include "gstreamer.h"
#include "pipewire.h"
#include <gio/gio.h>
#include <glib.h>
#include <gst/gst.h>
#include <pipewire/loop.h>
#include <pipewire/pipewire.h>

void
cb(guint32 node, gint fd)
{
    g_debug("[%u, %d]", node, fd);

    // launch pw
    pw_setup(node, fd);
}

int
main(int argc, char **argv)
{
    g_debug("Konnichiwa fellow debugger!");

    gst_init(&argc, &argv);
    pw_init(&argc, &argv);
    ex_gst_init(&argc, &argv);

    GMainLoop      *loop    = g_main_loop_new(NULL, FALSE);
    struct pw_loop *pw_loop = pw_create_loop();

    dbus_acquire_pipewire_node(cb);

    // event loop
    pw_loop_enter(pw_loop);
    g_main_loop_run(loop);
    pw_loop_leave(pw_loop);

    // exit
    g_main_loop_unref(loop);
    dbus_free();
    pw_free();

    return 0;
}
