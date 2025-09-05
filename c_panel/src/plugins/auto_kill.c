#include "log.h"
#include "plugins/plugins.h"
#include "state.h"
#include "utils.h"

void* plugin_auto_kill(void* _context)
{
    PluginState* context = _context;
    char buffer[32];
    string_set_cstr(context->data, " 20s ");

    for (int i = 20; i > 0 && running; i--) {
        snprintf(buffer, 32, " %ds ", i);
        string_set_cstr(context->data, buffer);
        panel_signal_render();
        msleep(1000);
    }

    logger_log(LOG_WARN, "auto_kill: killing panel");
    panel_stop();

    return NULL;
}
