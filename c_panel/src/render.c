#include "config.h"
#include "log.h"
#include "panel.h"
#include "plugins/plugins.h"
#include "string-utf8.h"
#include "utils.h"
#include <stdint.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

struct winsize terminal_size;

void render_init(void)
{
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal_size) == -1) {
        logger_log(LOG_ERROR, "failed to get terminal size");
        terminal_size.ws_col = 80;
        terminal_size.ws_row = 24;
    }
    // hide cursor
    printf("\033[?25l");
    logger_log(LOG_INFO, "terminal size: %dx%d", terminal_size.ws_col, terminal_size.ws_row);
}

void panel_render(void)
{
    logger_log(LOG_INFO, "rendering panel...");
    logger_log(LOG_DEBUG, "render start");

    // erase in line
    printf("\r\033[2K");

    // left plugins
    string* left_content = string_new(u8"");
    for (int i = 0; i < PLUGIN_LEFT_COUNT; i++) {
        PluginState state = plugin_states[i];
        string* old = left_content;

        left_content = string_concat(left_content, state.data);

        string_free(old);
    }
    char* left_cstr = string_cast(left_content);
    logger_log(LOG_DEBUG, "left content: %s (%zu chars, %zu bytes)", left_cstr, left_content->char_length, left_content->byte_length);

    // center plugins
    string* center_content = string_new(u8"");
    for (int i = 0; i < PLUGIN_CENTER_COUNT; i++) {
        PluginState state = plugin_states[PLUGIN_LEFT_COUNT + i];
        string* old = center_content;

        center_content = string_concat(center_content, state.data);

        string_free(old);
    }
    char* center_cstr = string_cast(center_content);
    logger_log(LOG_DEBUG, "center content: %s (%zu chars, %zu bytes)", center_cstr, center_content->char_length, center_content->byte_length);

    // right plugins
    string* right_content = string_new(u8"");
    for (int i = 0; i < PLUGIN_RIGHT_COUNT; i++) {
        PluginState state = plugin_states[PLUGIN_LEFT_COUNT + PLUGIN_CENTER_COUNT + i];
        string* old = right_content;

        right_content = string_concat(right_content, state.data);

        string_free(old);
    }
    char* right_cstr = string_cast(right_content);
    logger_log(LOG_DEBUG, "right content: %s (%zu chars, %zu bytes)", right_cstr, right_content->char_length, right_content->byte_length);

    // calculate spacing
    uint16_t center_padding = ((terminal_size.ws_col - center_content->char_length) / 2) - left_content->char_length;
    uint16_t right_padding = terminal_size.ws_col - (left_content->char_length + center_content->char_length + right_content->char_length + center_padding);
    logger_log(LOG_DEBUG, "padding: center=%d, right=%d", center_padding, right_padding);

    // display contents
    printf("%s", left_cstr);
    padding(center_padding);
    printf("%s", center_cstr);
    padding(right_padding);
    printf("%s", right_cstr);

    fflush(stdout);

    // free spree
    string_free(left_content);
    string_free(center_content);
    string_free(right_content);
    free(left_cstr);
    free(center_cstr);
    free(right_cstr);

    logger_log(LOG_DEBUG, "render end");
}
