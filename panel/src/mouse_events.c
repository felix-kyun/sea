#include "mouse_events.h"
#include "config/config.h"
#include "input_reader.h"
#include "log.h"
#include "modules/modules.h"
#include "panel.h"
#include "render.h"
#include <stdio.h>
#include <string.h>

static MouseEvent parse_mouse_event(const char* event)
{
    MouseEvent mevent = {
        .type = MOUSE_NONE,
        .x = -1
    };
    const char* start = strstr(event, "\033[<");

    if (!start)
        return mevent;

    int button, x;
    char action;

    if (sscanf(start, "\033[<%d;%d;%*d%c", &button, &x, &action) != 3)
        return mevent;

    mevent.x = x;

    // Button encoding:
    // 0: left press        (M)
    // 1: middle press      (M)
    // 2: right press       (M)
    // 64: scroll up        (M)
    // 65: scroll down      (M)
    // Button release:      (m)

    if (action == 'm')
        return mevent;

    switch (button) {
    case 0:
        mevent.type = MOUSE_LEFT_CLICK;
        break;
    case 1:
        mevent.type = MOUSE_MIDDLE_CLICK;
        break;
    case 2:
        mevent.type = MOUSE_RIGHT_CLICK;
        break;
    case 64:
        mevent.type = MOUSE_SCROLL_UP;
        break;
    case 65:
        mevent.type = MOUSE_SCROLL_DOWN;
        break;
    default:
        mevent.type = MOUSE_NONE;
        break;
    }

    return mevent;
}

static ModuleState* get_module_from_position(int x)
{
    int current_x = 0;

    // skip panel padding
    current_x += config.panel_padding;

    for (int i = 0; i < LEFT_COUNT; i++) {
        // check if x is within module bounds
        if (x > current_x && x <= current_x + (int)module_states[i].data->char_length) {
            return &module_states[i];
        }
        current_x += module_states[i].data->char_length;

        // add module spacing except after last module
        if (i < LEFT_COUNT - 1)
            current_x += config.module_spacing;
    }

    current_x += render_info.left_padding;
    if (x <= current_x)
        return NULL;

    for (int i = 0; i < CENTER_COUNT; i++) {
        if (x > current_x && x <= current_x + (int)module_states[LEFT_COUNT + i].data->char_length) {
            return &module_states[LEFT_COUNT + i];
        }
        current_x += module_states[LEFT_COUNT + i].data->char_length;

        if (i < CENTER_COUNT - 1)
            current_x += config.module_spacing;
    }

    current_x += render_info.right_padding;
    if (x <= current_x)
        return NULL;

    for (int i = 0; i < RIGHT_COUNT; i++) {
        if (x > current_x && x <= current_x + (int)module_states[LEFT_COUNT + CENTER_COUNT + i].data->char_length) {
            return &module_states[LEFT_COUNT + CENTER_COUNT + i];
        }
        current_x += module_states[LEFT_COUNT + CENTER_COUNT + i].data->char_length;

        if (i < RIGHT_COUNT - 1)
            current_x += config.module_spacing;
    }

    return NULL;
}

static inline void mouse_handle_event(const char* event)
{
    MouseEvent mevent = parse_mouse_event(event);
    if (mevent.type == MOUSE_NONE)
        return;

    DEBUG("mouse event: type=%d, x=%d", mevent.type, mevent.x);
    ModuleState* module = get_module_from_position(mevent.x);

    if (!module) {
        DEBUG("no module found at position %d", mevent.x);
        return;
    }

    switch (mevent.type) {
    case MOUSE_LEFT_CLICK:
        DEBUG("left click on module");
        if (module->on_left_click) {
            module->on_left_click(module);
        }
        break;
    case MOUSE_RIGHT_CLICK:
        DEBUG("right click on module");
        if (module->on_right_click)
            module->on_right_click(module);
        break;
    case MOUSE_MIDDLE_CLICK:
        DEBUG("middle click on module");
        if (module->on_middle_click)
            module->on_middle_click(module);
        break;
    case MOUSE_SCROLL_UP:
        DEBUG("scroll up on module");
        if (module->on_scroll_up)
            module->on_scroll_up(module);
        break;
    case MOUSE_SCROLL_DOWN:
        DEBUG("scroll down on module");
        if (module->on_scroll_down)
            module->on_scroll_down(module);
        break;
    default:
        break;
    }
}

void mouse_init(void)
{
    input_reader_init(mouse_handle_event);
}

void mouse_free(void)
{
    input_reader_free();
}
