#include "log.h"
#include "modules/modules.h"
#include "state.h"
#include "utils.h"

void* module_init(void* _context)
{
    ModuleState* context = _context;
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
