#include "log.h"
#include "plugins/plugins.h"

void* plugin_text(void* _context)
{
    PluginState* context = _context;
    logger_log(LOG_SUCCESS, "text plugin started");

    string_set_cstr(context->data, "Hello there");

    return NULL;
}
