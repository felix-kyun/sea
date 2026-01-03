#include "colors.h"
#include "modules/modules.h"
#include "utils.h"

void* module_init(void* _state)
{
    ModuleState* state = _state;

    // colors from config with defaults

    // const char* color = get_module_fg_color(state, "green");
    // const char* background = get_module_bg_color(state);

    // update module data
    // check ModuleState struct for more options
    string_set_cstr(state->data, "Hello, World!");

    // typical loop
    while (*state->running) {
        // perform periodic updates here
        msleep(1000); // sleep for 1 second
    }

    // used to render the updated data
    state->signal_render();
    return NULL;
}
