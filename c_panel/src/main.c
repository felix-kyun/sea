#include "log.h"
#include "overlay.h"
#include "panel.h"
#include "signal_handler.h"
#include "state.h"
#include <unistd.h>

int main(void)
{

    logger_init("panel.log", false);
    logger_log(LOG_SUCCESS, "starting sea panel with pid %d", getpid());
    setup_signal_handlers();
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
    logger_log(LOG_SUCCESS, "panel stopped");
    return 0;
}
