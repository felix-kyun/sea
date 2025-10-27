#include "signal_handler.h"
#include "config.h"
#include "log.h"
#include "modules/modules.h"
#include "render.h"
#include "state.h"
#include <signal.h>
#include <stdlib.h>

void handle_signal(int signum)
{
    DEBUG("received signal: %d", signum);
    // static int respawned = 0;
    // if (!respawned && signum == SIGHUP) {
    //     // restart panel when kitty gets closed
    //     respawned = 1;
    //     system(SPAWN_COMMAND);
    // }
    if (signum == SIGWINCH) {
        // terminal resized
        render_recalculate_size();
        panel_signal_render();
        return;
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

    // sigaction(SIGINT, &sa, NULL);
    // sigaction(SIGTERM, &sa, NULL);
    // sigaction(SIGKILL, &sa, NULL);
    // sigaction(SIGHUP, &sa, NULL);
    sigaction(SIGWINCH, &sa, NULL);
}
