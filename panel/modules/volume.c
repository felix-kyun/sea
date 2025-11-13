#include "colors.h"
#include "log.h"
#include "modules/modules.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VOLUME_ICON_HIGH " "
#define VOLUME_ICON_MED " "
#define VOLUME_ICON_LOW " "
#define VOLUME_ICON_MUTED "󰝟 "
#define VOLUME_COLOR BLUE
#define VOLUME_REFRESH 1000 // 1 second
#define VOLUME_DELTA 5

static char* color = "";
static char* background = "";
static char buffer[64];

static int get_volume(void)
{
    FILE* fp = popen("pactl get-sink-volume @DEFAULT_SINK@ 2>/dev/null | grep -oP '\\d+%' | head -1 | tr -d '%'", "r");
    if (!fp) {
        logger_log(LOG_ERROR, "failed to get volume");
        return -1;
    }

    int volume = 0;
    if (fscanf(fp, "%d", &volume) != 1) {
        pclose(fp);
        return -1;
    }

    pclose(fp);
    return volume;
}

static int is_muted(void)
{
    FILE* fp = popen("pactl get-sink-mute @DEFAULT_SINK@ 2>/dev/null | grep -q yes && echo 1 || echo 0", "r");
    if (!fp) {
        logger_log(LOG_ERROR, "failed to check mute status");
        return 0;
    }

    int muted = 0;
    if (fscanf(fp, "%d", &muted) != 1) {
        pclose(fp);
        return 0;
    }

    pclose(fp);
    return muted;
}

static char* get_volume_icon(int volume, int muted)
{
    if (muted) {
        return VOLUME_ICON_MUTED;
    } else if (volume >= 70) {
        return VOLUME_ICON_HIGH;
    } else if (volume >= 30) {
        return VOLUME_ICON_MED;
    } else {
        return VOLUME_ICON_LOW;
    }
}

static void set_volume_display(ModuleState* state, int volume, int muted)
{
    char* icon = get_volume_icon(volume, muted);
    snprintf(buffer, sizeof(buffer), "%s%s%s%d%%",
        background, color, icon, volume);
    string_set_cstr(state->data, buffer);
    state->signal_render();
}

static void change_volume(int delta)
{
    char cmd[128];
    if (delta > 0) {
        snprintf(cmd, sizeof(cmd), "pactl set-sink-volume @DEFAULT_SINK@ +%d%% 2>/dev/null", delta);
    } else {
        snprintf(cmd, sizeof(cmd), "pactl set-sink-volume @DEFAULT_SINK@ -- %d%% 2>/dev/null", delta);
    }
    system(cmd);
}

static void toggle_mute(void)
{
    system("pactl set-sink-mute @DEFAULT_SINK@ toggle 2>/dev/null");
}

static void on_scroll_up(ModuleState* state)
{
    (void)state;
    DEBUG("volume scroll up");
    change_volume(VOLUME_DELTA);
}

static void on_scroll_down(ModuleState* state)
{
    (void)state;
    DEBUG("volume scroll down");
    change_volume(-VOLUME_DELTA);
}

static void on_left_click(ModuleState* state)
{
    (void)state;
    DEBUG("volume toggle mute");
    toggle_mute();
}

void* module_init(void* _state)
{
    ModuleState* state = _state;
    color = get_fg_color(state->config_get(state->name, "color"), "blue");
    background = get_bg_color(state->config_get(state->name, "background"), "default");

    // set event handlers
    state->on_scroll_up = on_scroll_up;
    state->on_scroll_down = on_scroll_down;
    state->on_left_click = on_left_click;

    int volume = 0;
    int muted = 0;

    while (*state->running) {
        volume = get_volume();
        muted = is_muted();

        if (volume >= 0) {
            set_volume_display(state, volume, muted);
        }

        msleep(VOLUME_REFRESH);
    }

    return NULL;
}
