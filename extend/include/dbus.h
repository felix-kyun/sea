#pragma once

#include <gio/gio.h>
#include <glib.h>

#define BUS     "org.freedesktop.portal.Desktop"
#define OBJECT  "/org/freedesktop/portal/desktop"
#define IFACE   "org.freedesktop.portal.ScreenCast"
#define REQUEST "org.freedesktop.portal.Request"

void dbus_acquire_pipewire_session(GDBusConnection *conn, void (*cb)(guint32 node, gint fd));
void dbus_free();

#define define_handler_prototype(name)                                                                                 \
    void name##_handler(                                                                                               \
        [[maybe_unused]] GDBusConnection *conn,                                                                        \
        [[maybe_unused]] const gchar     *sender_name,                                                                 \
        [[maybe_unused]] const gchar     *object_path,                                                                 \
        [[maybe_unused]] const gchar     *interface_name,                                                              \
        [[maybe_unused]] const gchar     *signal_name,                                                                 \
        [[maybe_unused]] GVariant        *params,                                                                      \
        [[maybe_unused]] gpointer         user_data)

#define define_handler(name, body) define_handler_prototype(name) body

// Response handlers
define_handler_prototype(create_session);
define_handler_prototype(select_sources);
define_handler_prototype(start);

// Helper macros
#define dbus_call(conn, method, params, response_type, error)                                                          \
    g_dbus_connection_call_sync(                                                                                       \
        conn, BUS, OBJECT, IFACE, (method), (params), (response_type), G_DBUS_CALL_FLAGS_NONE, -1, NULL, error)

#define dbus_subscribe(conn, callback)                                                                                 \
    g_dbus_connection_signal_subscribe(                                                                                \
        conn, BUS, REQUEST, "Response", NULL, NULL, G_DBUS_SIGNAL_FLAGS_NONE, callback, NULL, NULL)
