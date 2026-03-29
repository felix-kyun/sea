#include "dbus.h"
#include "gst/gstelement.h"
#include "gst/gstmessage.h"
#include <gio/gio.h>
#include <glib.h>
#include <gst/gst.h>
#include <stdio.h>
#include <unistd.h>

#define PIPELINE "pipewiresrc path=%d fd=%d ! videoconvert ! xvimagesink"

GMainLoop *loop;

void
launch_gst(guint32 path, gint fd)
{
    g_autofree gchar *pipeline_str = g_strdup_printf(PIPELINE, path, fd);
    g_autoptr(GError) error        = NULL;
    g_autoptr(GstElement) pipeline = gst_parse_launch(pipeline_str, &error);
    if (error) {
        g_printerr("Failed to parse pipeline: %s", error->message);
        return;
    }
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    g_autoptr(GstBus) bus = gst_element_get_bus(pipeline);
    g_autoptr(GstMessage) msg
        = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
    if (msg && GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
        GError *err = NULL;
        gst_message_parse_error(msg, &err, NULL);
        g_debug("Error: %s", err->message);
        g_error_free(err);
    }
    gst_element_set_state(pipeline, GST_STATE_NULL);
    g_debug("Sayonara :3");
}

int
main(int argc, char **argv)
{
    g_debug("Konnichiwa fellow debugger!");
    gst_init(&argc, &argv);
    g_autoptr(GError) error = NULL;
    loop                    = g_main_loop_new(NULL, FALSE);
    GDBusConnection *conn   = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);
    if (!conn) {
        g_printerr("Failed to get session bus: %s", error->message);
        return 1;
    }

    dbus_acquire_pipewire_session(conn, launch_gst);

    g_main_loop_run(loop);
    g_main_loop_unref(loop);
    dbus_free();
    return 0;
}
