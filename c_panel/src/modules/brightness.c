#define _DEFAULT_SOURCE
#include "colors.h"
#include "log.h"
#include "modules/modules.h"
#include <dirent.h>
#include <linux/limits.h>
#include <sys/inotify.h>
#include <unistd.h>

#define BRIGHTNESS_ICON " "
#define BRIGHTNESS_COLOR GREEN
#define BRIGHTNESS_PATH "/sys/class/backlight/"

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

static char max_brightness_path[PATH_MAX] = { 0 };
static char current_brightness_path[PATH_MAX] = { 0 };
static int max_brightness = 0;

static int init_brightness_paths(void)
{
    struct dirent* entry;
    DIR* dp = opendir(BRIGHTNESS_PATH);
    if (dp == NULL) {
        logger_log(LOG_ERROR, "failed to open directory: " BRIGHTNESS_PATH);
        return -1;
    }

    while ((entry = readdir(dp))) {
        if ((entry->d_type == DT_DIR || entry->d_type == DT_LNK) && entry->d_name[0] != '.') {
            snprintf(max_brightness_path, PATH_MAX, "%s%s/max_brightness", BRIGHTNESS_PATH, entry->d_name);
            snprintf(current_brightness_path, PATH_MAX, "%s%s/brightness", BRIGHTNESS_PATH, entry->d_name);

            logger_log(LOG_SUCCESS, "found brightness files:");
            logger_log(LOG_SUCCESS, "max: %s", max_brightness_path);
            logger_log(LOG_SUCCESS, "current: %s", current_brightness_path);

            closedir(dp);
            return 0;
        }
    }

    closedir(dp);
    return -1;
}

static void set_brightness(ModuleState* state, int brightness)
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), BRIGHTNESS_COLOR BRIGHTNESS_ICON " %d%% " RESET, (brightness * 100) / max_brightness);
    string_set_cstr(state->data, buffer);
    panel_signal_render();
}

static int get_max_brightness(void)
{
    FILE* file;
    int max_brightness = 0;

    file = fopen(max_brightness_path, "r");
    if (file == NULL) {
        logger_log(LOG_ERROR, "failed to open max brightness file: %s", max_brightness_path);
        return -1;
    }
    if (fscanf(file, "%d", &max_brightness) != 1) {
        logger_log(LOG_ERROR, "failed to read max brightness value");
        fclose(file);
        return -1;
    }
    fclose(file);

    return max_brightness;
}

static int get_current_brightness(void)
{
    FILE* file;
    int current_brightness = 0;

    file = fopen(current_brightness_path, "r");
    if (file == NULL) {
        logger_log(LOG_ERROR, "failed to open current brightness file: %s", current_brightness_path);
        return -1;
    }
    if (fscanf(file, "%d", &current_brightness) != 1) {
        logger_log(LOG_ERROR, "failed to read current brightness value");
        fclose(file);
        return -1;
    }
    fclose(file);

    return current_brightness;
}

void* module_brightness(void* _state)
{
    ModuleState* state = _state;
    int ret = init_brightness_paths();
    if (ret < 0) {
        logger_log(LOG_ERROR, "failed to initialize brightness paths");
        return NULL;
    }

    max_brightness = get_max_brightness();
    if (max_brightness <= 0) {
        logger_log(LOG_ERROR, "invalid max brightness value");
        return NULL;
    }

    // create watcher
    int inotify_fd = inotify_init();
    if (inotify_fd < 0) {
        logger_log(LOG_ERROR, "failed to initialize inotify");
        return NULL;
    }
    int wd = inotify_add_watch(inotify_fd, current_brightness_path, IN_MODIFY);
    if (wd < 0) {
        logger_log(LOG_ERROR, "failed to add inotify watch for file: %s", current_brightness_path);
        close(inotify_fd);
    }

    set_brightness(state, get_current_brightness());

    char buffer[BUF_LEN];
    while (1) {
        int length = read(inotify_fd, buffer, BUF_LEN);
        if (length < 0) {
            logger_log(LOG_ERROR, "failed to read inotify events");
            break;
        }

        int i = 0;
        while (i < length) {
            struct inotify_event* event = (struct inotify_event*)&buffer[i];
            if (event->mask & IN_MODIFY) {
                DEBUG("brightness file modified");
                int current_brightness = get_current_brightness();
                if (current_brightness >= 0) {
                    set_brightness(state, current_brightness);
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }

    inotify_rm_watch(inotify_fd, wd);
    close(inotify_fd);
    return NULL;
}
