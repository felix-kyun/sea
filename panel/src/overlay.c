#include "overlay.h"
#include "colors.h"
#include "log.h"
#include "modules/modules.h"
#include "state.h"
#include "string-utf8.h"
#include "utils.h"
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_PATH "/tmp/sea-notify-pipe"
#define NOTIFY_COLOR BOLD_BLUE
#define NOTIFY_ICON "󰂚 "
#define NOTIFY_LIMIT 100

Overlay overlay;
char buffer[256];
char print_buffer[256] = { 0 };

void overlay_init(void)
{
    overlay.content = string_new("");
    overlay.active = false;
    overlay.cleaner = 0;
    overlay.watcher = 0;
    pthread_mutex_init(&overlay.lock, NULL);
    DEBUG("overlay initialized");

    unlink(FIFO_PATH);
    int ret = mkfifo(FIFO_PATH, 0666);

    if (ret == -1) {
        logger_log(LOG_ERROR, "failed to create fifo at %s", FIFO_PATH);
    } else if (ret == 0) {
        logger_log(LOG_SUCCESS, "created fifo at %s", FIFO_PATH);
    } else {
        logger_log(LOG_INFO, "fifo already exists at %s", FIFO_PATH);
    }

    // for the future me
    // if you get any issues with not hitting EOF
    // its because the we are the writers too
    overlay.fifo_fd = open(FIFO_PATH, O_RDWR);

    if (overlay.fifo_fd == -1) {
        logger_log(LOG_ERROR, "failed to open fifo at %s", FIFO_PATH);
    } else {
        logger_log(LOG_SUCCESS, "opened fifo at %s", FIFO_PATH);
    }
}

void overlay_set(const char* content)
{
    DEBUG("overlay set called");
    pthread_mutex_lock(&overlay.lock);
    DEBUG("setting overlay content: %s", content);

    // remove any existing cleaner
    if (overlay.active)
        pthread_cancel(overlay.cleaner);

    // set new content
    string_set_cstr(overlay.content, content);

    // spawn new cleaner
    overlay.active = true;
    pthread_create(&overlay.cleaner, NULL, overlay_cleaner, (void*)&overlay);

    pthread_mutex_unlock(&overlay.lock);
    DEBUG("overlay set complete");
}

void* overlay_cleaner(void* _overlay)
{
    Overlay* overlay = _overlay;
    DEBUG("overlay cleaner started");

    sleep(5);

    pthread_mutex_lock(&overlay->lock);
    string_set_cstr(overlay->content, "");
    overlay->active = false;
    panel_signal_render();
    pthread_mutex_unlock(&overlay->lock);

    DEBUG("overlay cleared by cleaner");
    return NULL;
}

void overlay_free(void)
{
    DEBUG("freeing overlay");
    pthread_mutex_lock(&overlay.lock);
    pthread_cancel(overlay.watcher);

    if (overlay.active)
        pthread_cancel(overlay.cleaner);

    string_free(overlay.content);
    close(overlay.fifo_fd);
    unlink(FIFO_PATH);

    pthread_mutex_unlock(&overlay.lock);
    pthread_mutex_destroy(&overlay.lock);
}

void* overlay_watcher(void* _overlay)
{
    Overlay* overlay = _overlay;

    while (running) {
        ssize_t bytes_read = read(overlay->fifo_fd, buffer, 256 - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            overlay_set(buffer);
            panel_signal_render();
        } else if (bytes_read == -1) {
            logger_log(LOG_ERROR, "error reading from fifo");
            logger_log(LOG_ERROR, "errno: %d", errno);
            logger_log(LOG_ERROR, "error: %s", strerror(errno));
            break;
        }
    }

    return NULL;
}

void overlay_print(unsigned short terminal_width)
{
    // only run if content has changed
    if (memcmp(print_buffer, overlay.content->data, overlay.content->byte_length) == 0) {
        DEBUG("overlay content unchanged, skipping print");
        return;
    }

    DEBUG("printing overlay content");

    memcpy(print_buffer, overlay.content->data, overlay.content->byte_length);

    int req_padding = ((terminal_width - overlay.content->char_length + 2) / 2);
    if (req_padding < 0)
        req_padding = 0;

    printf("\r\033[2K");
    padding(req_padding);
    printf(NOTIFY_COLOR NOTIFY_ICON);
    fwrite(overlay.content->data, sizeof(char),
        (overlay.content->char_length > NOTIFY_LIMIT)
            ? NOTIFY_LIMIT
            : overlay.content->byte_length,
        stdout);
    printf(RESET);

    fflush(stdout);
}

void overlay_spawn_watcher(void)
{
    pthread_create(&overlay.watcher, NULL, overlay_watcher, (void*)&overlay);
}
