#include "config/config.h"
#include "log.h"
#include "mouse_events.h"
#include "overlay.h"
#include "panel.h"
#include "signal_handler.h"
#include "state.h"
#include "utils.h"
#include <unistd.h>

int main(int argc, char** argv)
{
    create_app_config(argc, argv);

    logger_init(config.log_file, config.log_to_stdout);
    debug_config();
    logger_log(LOG_SUCCESS, "starting sea panel with pid %d", getpid());

    setup_signal_handlers();
    mouse_init();
    panel_init();
    panel_init_modules();
    overlay_init();
    overlay_spawn_watcher();

    // render loop
    while (running) {
        // wait for render signal
        pthread_mutex_lock(&render_signal.mutex);
        pthread_cond_wait(&render_signal.cond, &render_signal.mutex);

        // render panel
        panel_render();

        pthread_mutex_unlock(&render_signal.mutex);
    }

    overlay_free();
    panel_free();
    mouse_free();
    logger_log(LOG_SUCCESS, "panel stopped");
    return 0;
}
