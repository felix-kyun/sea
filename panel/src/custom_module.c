#include "modules/custom_module.h"
#include <stdlib.h>

void text_module_cleanup(TextModule* module)
{
    if (module) {
        if (module->content) {
            free(module->content);
        }
        free(module);
    }
}
