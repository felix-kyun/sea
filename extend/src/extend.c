#include "gst/gstelement.h"
#include "gst/gstmessage.h"
#include <gio/gio.h>
#include <glib.h>
#include <gst/gst.h>
#include <stdio.h>
#include <unistd.h>

#define PIPELINE "pipewiresrc path=%d fd=%d ! videoconvert ! xvimagesink"

#define BUS     "org.freedesktop.portal.Desktop"
#define OBJECT  "/org/freedesktop/portal/desktop"
#define IFACE   "org.freedesktop.portal.ScreenCast"
#define REQUEST "org.freedesktop.portal.Request"

#define dbus_call(conn, method, params, response_type, error)                                                          \
    g_dbus_connection_call_sync(                                                                                       \
        conn, BUS, OBJECT, IFACE, (method), (params), (response_type), G_DBUS_CALL_FLAGS_NONE, -1, NULL, error)

#define dbus_subscribe(conn, callback)                                                                                 \
    g_dbus_connection_signal_subscribe(                                                                                \
        conn, BUS, REQUEST, "Response", NULL, NULL, G_DBUS_SIGNAL_FLAGS_NONE, callback, NULL, NULL)

GMainLoop       *loop;
GDBusConnection *g_dbus_conn;
gchar           *session_handle = NULL;

void
pw_node_debug(gint node_id, GVariant *props)
{
    GVariantIter iter;
    const gchar *key;
    GVariant    *value;
    g_variant_iter_init(&iter, props);
    g_debug("Node %d: ", node_id);
    while (g_variant_iter_loop(&iter, "{sv}", &key, &value)) {
        gchar *str = g_variant_print(value, TRUE);
        g_debug("  %s: %s", key, str);
        g_free(str);
    }
}

void
launch_gst(int path, int fd)
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
    g_debug("Sayonara!");
}

void
start_response_handler(
    GDBusConnection *conn,
    const gchar     *sender_name,
    const gchar     *object_path,
    const gchar     *interface_name,
    const gchar     *signal_name,
    GVariant        *params,
    gpointer         user_data)
{
    g_autofree gchar *create_session_request_token = g_strdup_printf("Start_request_%d", getpid());
    if (!g_str_has_suffix(object_path, create_session_request_token)) {
        return;
    }

    guint32 response;
    g_autoptr(GVariant) results = NULL;
    g_variant_get(params, "(u@a{sv})", &response, &results);
    if (!results) {
        return;
    }

    g_autoptr(GVariant) streams = g_variant_lookup_value(results, "streams", G_VARIANT_TYPE("a(ua{sv})"));
    if (!streams) {
        g_error("No streams found in response");
        return;
    }

    // iterate
    GVariantIter iter;
    g_variant_iter_init(&iter, streams);
    guint32   node_id;
    GVariant *props = NULL;

    while (g_variant_iter_loop(&iter, "(u@a{sv})", &node_id, &props)) {
        pw_node_debug(node_id, props);
        GVariantBuilder builder;
        g_variant_builder_init(&builder, G_VARIANT_TYPE("a{sv}"));
        g_variant_builder_add(&builder, "{sv}", "node-id", g_variant_new_uint32(node_id));
        g_autoptr(GError) error = NULL;
        GUnixFDList *fd_list    = NULL;

        g_autoptr(GVariant) out = g_dbus_connection_call_with_unix_fd_list_sync(
            conn,
            BUS,
            OBJECT,
            IFACE,
            "OpenPipeWireRemote",
            g_variant_new("(oa{sv})", g_strdup(session_handle), &builder),
            G_VARIANT_TYPE("(h)"),
            G_DBUS_CALL_FLAGS_NONE,
            -1,
            fd_list,
            &fd_list,
            NULL,
            &error);
        if (error) {
            g_warning("[request] OpenPipeWireRemote: %s", error->message);
            return;
        }
        if (!fd_list) {
            g_error("No FD list returned!\n");
            return;
        }

        gint fd_index;
        g_variant_get(out, "(h)", &fd_index);
        gint _fd = g_unix_fd_list_get(fd_list, fd_index, &error);
        gint fd  = dup(_fd);

        g_print("fd_index=%d _fd=%d fd=%d\n", fd_index, _fd, fd);
        if (error || fd < 0) {
            g_error("fd extraction failed: %s", error ? error->message : "unknown error");
            return;
        }

        g_debug("[success] Acquired node_id:fd=%d:%d", node_id, fd);
        // g_main_loop_quit(loop);
        launch_gst(node_id, fd);
    }
}

void
select_sources_response_handler(
    GDBusConnection *conn,
    const gchar     *sender_name,
    const gchar     *object_path,
    const gchar     *interface_name,
    const gchar     *signal_name,
    GVariant        *params,
    gpointer         user_data)
{
    g_autofree gchar *create_session_request_token = g_strdup_printf("SelectSources_request_%d", getpid());
    if (!g_str_has_suffix(object_path, create_session_request_token)) {
        return;
    }

    guint32 response;
    g_autoptr(GVariant) results = NULL;
    g_variant_get(params, "(u@a{sv})", &response, &results);
    if (!results) {
        return;
    }
    gchar          *start_request_token = g_strdup_printf("Start_request_%d", getpid());
    gchar          *parent_window       = g_strdup_printf("");
    GVariantBuilder builder;
    g_variant_builder_init(&builder, G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_add(&builder, "{sv}", "handle_token", g_variant_new_string(start_request_token));

    dbus_subscribe(conn, start_response_handler);

    g_autoptr(GError) error = NULL;
    g_autoptr(GVariant) out = dbus_call(
        conn,
        "Start",
        g_variant_new("(osa{sv})", session_handle, parent_window, &builder),
        G_VARIANT_TYPE("(o)"),
        &error);
    if (error) {
        g_warning("[request] Start: %s", error->message);
        return;
    }
    const gchar *request_handle = NULL;
    g_variant_get(out, "(&o)", &request_handle);
    g_debug("[request] Start: %s", request_handle);
}

void
create_session_response_handler(
    GDBusConnection *conn,
    const gchar     *sender_name,
    const gchar     *object_path,
    const gchar     *interface_name,
    const gchar     *signal_name,
    GVariant        *params,
    gpointer         user_data)
{
    g_autofree gchar *create_session_request_token = g_strdup_printf("CreateSession_request_%d", getpid());
    if (!g_str_has_suffix(object_path, create_session_request_token)) {
        return;
    }

    guint32 response;
    g_autoptr(GVariant) results = NULL;
    g_variant_get(params, "(u@a{sv})", &response, &results);
    if (!results) {
        return;
    }
    gchar *tmp = NULL;
    if (g_variant_lookup(results, "session_handle", "&s", &tmp) == FALSE) {
        return;
    }
    session_handle = g_strdup(tmp);
    g_debug("[session] Acquired: %s", session_handle);

    g_autofree gchar *select_sources_request_token = g_strdup_printf("SelectSources_request_%d", getpid());
    GVariantBuilder   builder;
    g_variant_builder_init(&builder, G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_add(&builder, "{sv}", "handle_token", g_variant_new_string(select_sources_request_token));
    g_variant_builder_add(&builder, "{sv}", "types", g_variant_new_uint32(1 | 2));
    g_variant_builder_add(&builder, "{sv}", "persist_mode", g_variant_new_uint32(2));
    g_variant_builder_add(&builder, "{sv}", "multiple", g_variant_new_boolean(FALSE));

    dbus_subscribe(conn, select_sources_response_handler);
    g_autoptr(GError) error = NULL;
    g_autoptr(GVariant) out = dbus_call(
        conn, "SelectSources", g_variant_new("(oa{sv})", session_handle, &builder), G_VARIANT_TYPE("(o)"), &error);
    if (error) {
        g_printerr("Failed to call SelectSources: %s", error->message);
        return;
    }
    const gchar *request_handle = NULL;
    g_variant_get(out, "(&o)", &request_handle);
    g_debug("[request] SelectSources: %s", request_handle);
}

void
create_session()
{
    g_autofree gchar *req_token     = g_strdup_printf("CreateSession_request_%d", getpid());
    g_autofree gchar *session_token = g_strdup_printf("session_%d", getpid());
    GVariantBuilder   opts;
    g_variant_builder_init(&opts, G_VARIANT_TYPE("a{sv}"));
    g_variant_builder_add(&opts, "{sv}", "handle_token", g_variant_new_string(req_token));
    g_variant_builder_add(&opts, "{sv}", "session_handle_token", g_variant_new_string(session_token));

    dbus_subscribe(g_dbus_conn, create_session_response_handler);

    // call CreateSession
    g_autoptr(GError) error = NULL;
    g_autoptr(GVariant) out
        = dbus_call(g_dbus_conn, "CreateSession", g_variant_new("(a{sv})", &opts), G_VARIANT_TYPE("(o)"), &error);
    if (error) {
        g_printerr("Failed to create session: %s", error->message);
        return;
    }
    gchar *request_handle = NULL;
    g_variant_get(out, "(&o)", &request_handle);
    g_debug("[request] CreateSession: %s", request_handle);
}

int
main(int argc, char **argv)
{
    g_debug("Konnichiwa!");
    gst_init(&argc, &argv);
    g_autoptr(GError) error = NULL;
    loop                    = g_main_loop_new(NULL, FALSE);
    g_dbus_conn             = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);
    if (!g_dbus_conn) {
        g_printerr("Failed to get session bus: %s", error->message);
        return 1;
    }
    g_debug("Session bus connected.");

    create_session();

    g_main_loop_run(loop);

    g_free(session_handle);
    g_main_loop_unref(loop);
    return 0;
}
