#include "input_reader.h"
#include "log.h"
#include <pthread.h>
#include <termios.h>
#include <unistd.h>

struct termios original_termios;
pthread_t reader_thread;

static inline void set_raw_mode(void)
{
    struct termios term;
    tcgetattr(STDIN_FILENO, &original_termios);
    term = original_termios;
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

static inline void reset_terminal_mode(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

static inline void enable_mouse_reporting(void)
{
    printf("\033[?1000h");
    printf("\033[?1006h");
    fflush(stdout);
}

static inline void disable_mouse_reporting(void)
{
    printf("\033[?1000l");
    printf("\033[?1006l");
    fflush(stdout);
}
void* reader_routine(void* arg)
{
    input_callback event_handler = (input_callback)arg;
    char buffer[256];
    size_t n = 0;

    while ((n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        DEBUG("input read by reader");
        event_handler(buffer);
    }

    return NULL;
}

void input_reader_init(input_callback handler)
{
    set_raw_mode();
    enable_mouse_reporting();

    pthread_create(&reader_thread, NULL, reader_routine, (void*)handler);
    pthread_detach(reader_thread);

    logger_log(LOG_INFO, "input reader initialized");
}

void input_reader_free(void)
{
    pthread_cancel(reader_thread);

    reset_terminal_mode();
    disable_mouse_reporting();
    logger_log(LOG_INFO, "input reader freed");
}
