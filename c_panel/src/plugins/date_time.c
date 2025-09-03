#include "log.h"
#include "panel.h"
#include "utils.h"
#include <stdlib.h>

void* plugin_date_time(void* _context)
{
    PluginContext* context = (PluginContext*)_context;
    logger_log(LOG_SUCCESS, "date_time plugin started");

    char* buffer = malloc(20);

    while (1) {
        time_t now = time(NULL);
        struct tm* t = localtime(&now);
        strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", t);

        string_free(context->state->data);
        context->state->data = string_new(buffer);

        panel_signal_render();
        msleep(1000);
    }

    free(buffer);
    return NULL;
}
