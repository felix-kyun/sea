#pragma once
#include <glib.h>
#include <glibconfig.h>
#include <pipewire/pipewire.h>

struct pw_loop *pw_create_loop();
void            pw_setup(guint32 node, gint fd);
void            pw_free();
gboolean        pw_dispatch_handler(GSource *source, GSourceFunc callback, gpointer user_data);
