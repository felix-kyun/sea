#include "log.h"
#include "plugins/plugins.h"
#include "state.h"
#include "utils.h"

void* plugin_auto_kill(void* _context)
{
    PluginState* context = _context;
    string_set_cstr(context->data, "Auto kill in 20 seconds...");

    msleep(20000);
    logger_log(LOG_WARN, "auto killing panel after 20 seconds");
    panel_stop();

    return _context;
}
