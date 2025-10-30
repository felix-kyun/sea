#include "config_watcher.h"
#include "log.h"
#include "state.h"
#include <linux/limits.h>
#include <pthread.h>
#include <sys/inotify.h>
#include <unistd.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + NAME_MAX + 1))
#define WATCH_MASK IN_MODIFY | IN_CLOSE_WRITE | IN_MOVE_SELF | IN_DELETE_SELF

void* config_watcher(void* arg)
{
    const char* config_path = (const char*)arg;

    int inotify_fd = inotify_init();
    if (inotify_fd < 0) {
        logger_log(LOG_ERROR, "failed to initialize inotify");
        return NULL;
    }

    int wd = inotify_add_watch(inotify_fd, config_path, WATCH_MASK);
    if (wd < 0) {
        logger_log(LOG_ERROR, "failed to add inotify watch for file: %s", config_path);
        close(inotify_fd);
        return NULL;
    }

    logger_log(LOG_INFO, "watching config file for changes: %s", config_path);

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
            if (event->mask & WATCH_MASK) {
                DEBUG("config file modified: %s", config_path);
                panel_stop();
                inotify_rm_watch(inotify_fd, wd);
                close(inotify_fd);
                return NULL;
            }
            i += EVENT_SIZE + event->len;
        }
    }

    return NULL;
}

void create_config_watcher(const char* config_path)
{
    pthread_t thread;
    pthread_create(&thread, NULL, config_watcher, (void*)config_path);
    pthread_detach(thread);
}
