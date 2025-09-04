#include "log.h"
#include "state.h"
#include "utils.h"

void* plugin_auto_kill(void* _context)
{
    (void)_context;

    msleep(5000);
    logger_log(LOG_WARN, "auto killing panel after 5 seconds");
    panel_stop();

    return _context;
}
