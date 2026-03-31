#pragma once
#include <glib.h>
#include <glibconfig.h>
#include <pipewire/pipewire.h>

struct pw_loop *pw_create_loop();
void            pw_setup(guint32 node, gint fd);
void            pw_free();
gboolean        pw_dispatch_handler(GSource *source, GSourceFunc callback, gpointer user_data);
void            on_param_changed(void *userdata, uint32_t id, const struct spa_pod *param);
void            on_process(void *userdata);
void on_state_changed(void *userdata, enum pw_stream_state old, enum pw_stream_state state, const char *error);
