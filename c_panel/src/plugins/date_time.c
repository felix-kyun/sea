#include "plugins/plugins.h"
#include "state.h"
#include "utils.h"
#include <stdlib.h>
#include <time.h>

void* plugin_date_time(void* _context)
{
    PluginState* context = _context;

    char* buffer = malloc(20);

    while (running) {
        time_t now = time(NULL);
        struct tm* t = localtime(&now);
        strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", t);

        string_free(context->data);
        context->data = string_new(buffer);

        panel_signal_render();
        msleep(1000);
    }

    free(buffer);
    return NULL;
}
