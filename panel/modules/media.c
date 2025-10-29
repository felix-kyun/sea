#include "colors.h"
#include "log.h"
#include "modules/modules.h"
#include "state.h"
#include "utils.h"
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MEDIA_COLOR MAGENTA
#define MEDIA_FORMAT "{{ title }}"
#define MEDIA_ICON "󰎆 "
#define LIMIT 60

pid_t pid = 0;

// cleanup function
void module_media_stop(void)
{
    if (pid != 0) {
        kill(pid, SIGTERM);
        pid = 0;
    }
}

void module_on_click(ModuleState* state)
{
    (void)state;
    DEBUG("media clicked, toggling play/pause");
    system("playerctl play-pause");
}

void module_on_scroll_up(ModuleState* state)
{
    (void)state;
    DEBUG("media scrolled up, next track");
    system("playerctl next");
}

void module_on_scroll_down(ModuleState* state)
{
    (void)state;
    DEBUG("media scrolled down, previous track");
    system("playerctl previous");
}

static void ascii(const char* data)
{
    if (!data)
        return;

    size_t len = strlen(data);
    char* result = (char*)malloc((len + 1) * sizeof(char));
    size_t j = 0;

    for (size_t i = 0; i < len; i++) {
        if ((unsigned char)data[i] <= 0x7F) { // ASCII character
            result[j++] = data[i];
        }
    }
    result[j] = '\0';

    memcpy((char*)data, result, j + 1);
    free(result);
}

void* module_init(void* _state)
{
    ModuleState* state = _state;
    int pipe_fd[2];

    // register cleanup function
    state->cleanup = module_media_stop;
    state->on_left_click = module_on_click;
    state->on_scroll_up = module_on_scroll_up;
    state->on_scroll_down = module_on_scroll_down;

    // create pipe and fork
    pipe(pipe_fd);
    pid = fork();

    if (pid == -1) {
        logger_log(LOG_ERROR, "failed to fork media plugin");
        return NULL;
    }

    // child process
    if (pid == 0) {
        // redirect stdout to pipe
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[0]);
        execlp("stdbuf", "stdbuf", "-oL", "playerctl", "metadata", "--format", MEDIA_FORMAT, "-F", (char*)NULL);

        logger_log(LOG_ERROR, "failed to exec playerctl");
        exit(1);
    }

    close(pipe_fd[1]);
    FILE* pipe = fdopen(pipe_fd[0], "r");
    if (!pipe) {
        logger_log(LOG_ERROR, "failed to open pipe for media plugin");
    }

    char buffer[128];
    while (running && (fgets(buffer, sizeof(buffer), pipe) != NULL)) {
        DEBUG("media output: %s", buffer);
        // remove newline
        buffer[strcspn(buffer, "\n")] = 0;

        if (strlen(buffer) == 0) {
            string_set_cstr(state->data, "");
        } else {
            char media_buffer[128];
            ascii(buffer);
            string_limit(buffer, LIMIT);
            snprintf(media_buffer, sizeof(media_buffer), MEDIA_COLOR " " MEDIA_ICON "%s " RESET, buffer);
            string_set_cstr(state->data, media_buffer);
        }
    }

    return NULL;
}
