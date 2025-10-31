#include "colors.h"
#include "modules/custom_module.h"
#include "modules/modules.h"
#include <stdio.h>

void* module_init(void* _state)
{
    ModuleState* state = _state;
    TextModule* module = (TextModule*)state->custom_data;
    const char* color = get_fg_color(state->config_get(state->name, "color"), "white");
    const char* background = get_bg_color(state->config_get(state->name, "background"), "default");
    char buffer[256];

    snprintf(buffer, 256, "%s%s %s" RESET, color, background, module->content);

    string_set_cstr(state->data, buffer);
    state->signal_render();

    return NULL;
}
