#include "log.h"
#include "plugins/plugins.h"

void* plugin_text(void* _context)
{
    PluginState* context = _context;
    logger_log(LOG_SUCCESS, "text plugin started");

    context->data = string_new("Hello there");

    return NULL;
}
