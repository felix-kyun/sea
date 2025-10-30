#include "modules/custom_module.h"
#include "modules/modules.h"

void* module_init(void* _state)
{
    ModuleState* state = _state;
    TextModule* module = (TextModule*)state->custom_data;

    string_set_cstr(state->data, module->content);
    state->signal_render();

    return NULL;
}
