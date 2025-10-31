#include "render.h"
#include "config/config.h"
#include "log.h"
#include "modules/modules.h"
#include "overlay.h"
#include "panel.h"
#include "string-utf8.h"
#include "utils.h"
#include <stdint.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

struct winsize terminal_size;
RenderInfo render_info;

void render_recalculate_size(void)
{
    DEBUG("recalculating terminal size");
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal_size) == -1) {
        logger_log(LOG_ERROR, "failed to get terminal size");
        terminal_size.ws_col = 80;
        terminal_size.ws_row = 24;
    }
}

void render_init(void)
{
    render_recalculate_size();
    // hide cursor
    printf("\033[?25l");
    logger_log(LOG_INFO, "terminal size: %dx%d", terminal_size.ws_col, terminal_size.ws_row);
}

void panel_render(void)
{
    if (overlay.active) {
        DEBUG("overlay active, rendering overlay");
        overlay_print(terminal_size.ws_col);
        return;
    }

    DEBUG("render start");

    // erase in line
    printf("\r\033[2K");

    // module spacing
    char* module_spacing_str = create_padding_str(config.module_spacing);
    string* module_spacing_string = string_new(module_spacing_str);
    free(module_spacing_str);

    // panel padding
    char* panel_padding_str = create_padding_str(config.panel_padding);

    // left plugins
    string* left_content = string_new(panel_padding_str);
    for (int i = 0; i < LEFT_COUNT; i++) {
        ModuleState state = module_states[i];
        string* old = left_content;

        left_content = string_concat(left_content, state.data);
        string_free(old);

        // add padding if not last module
        if (i < LEFT_COUNT - 1) {
            old = left_content;
            left_content = string_concat(left_content, module_spacing_string);
            string_free(old);
        }
    }
    char* left_cstr = string_cast(left_content);

    // center modules
    string* center_content = string_new("");
    for (int i = 0; i < CENTER_COUNT; i++) {
        ModuleState state = module_states[LEFT_COUNT + i];
        string* old = center_content;

        center_content = string_concat(center_content, state.data);
        string_free(old);

        if (i < CENTER_COUNT - 1) {
            old = center_content;
            center_content = string_concat(center_content, module_spacing_string);
            string_free(old);
        }
    }
    char* center_cstr = string_cast(center_content);

    // right modules
    string* right_content = string_new("");
    for (int i = 0; i < RIGHT_COUNT; i++) {
        ModuleState state = module_states[LEFT_COUNT + CENTER_COUNT + i];
        string* old = right_content;

        right_content = string_concat(right_content, state.data);
        string_free(old);

        if (i < RIGHT_COUNT - 1) {
            old = right_content;
            right_content = string_concat(right_content, module_spacing_string);
            string_free(old);
        }
    }
    string* old = right_content;
    string* padding_right = string_new(panel_padding_str);
    right_content = string_concat(right_content, padding_right);
    char* right_cstr = string_cast(right_content);
    free(panel_padding_str);
    string_free(padding_right);
    string_free(old);

    // calculate spacing
    render_info.left_padding = ((terminal_size.ws_col - center_content->char_length) / 2) - left_content->char_length;
    render_info.right_padding = terminal_size.ws_col - (left_content->char_length + center_content->char_length + right_content->char_length + render_info.left_padding);

    // display contents
    printf("%s", left_cstr);
    padding(render_info.left_padding);
    printf("%s", center_cstr);
    padding(render_info.right_padding);
    printf("%s", right_cstr);

    fflush(stdout);

    // free spree
    string_free(left_content);
    string_free(center_content);
    string_free(right_content);
    string_free(module_spacing_string);
    free(left_cstr);
    free(center_cstr);
    free(right_cstr);

    DEBUG("render end");
}
