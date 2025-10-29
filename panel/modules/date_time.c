#include "colors.h"
#include "modules/modules.h"
#include "state.h"
#include "string-utf8.h"
#include "utils.h"
#include <stdlib.h>
#include <time.h>

void* module_init(void* _context)
{
    ModuleState* context = _context;
    char* buffer = malloc(64);

    while (running) {
        time_t now = time(NULL);
        struct tm* t = localtime(&now);
        strftime(buffer, 64, YELLOW " %H:%M:%S " GREEN " 󰃰 %a %b %d %Y " RESET, t);

        // only signal render if the time has changed
        if (!string_equals_cstr(context->data, buffer)) {
            string_set_cstr(context->data, buffer);
            panel_signal_render();
        }

        msleep(1000);
    }

    free(buffer);
    return NULL;
}
