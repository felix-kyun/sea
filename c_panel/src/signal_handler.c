#include "signal_handler.h"
#include "config.h"
#include "plugins/plugins.h"
#include "state.h"
#include <signal.h>
#include <stdlib.h>

void handle_signal(int signum)
{
    if (signum == SIGHUP) {
        // restart panel when kitty gets closed
        system(SPAWN_COMMAND);
    }

    running = false;
    panel_signal_render();
}

void setup_signal_handlers(void)
{
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGKILL, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
}
