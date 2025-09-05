#include "log.h"
#include "panel.h"
#include "state.h"
#include <unistd.h>

int main(void)
{

    logger_init("panel.log", false);
    logger_log(LOG_SUCCESS, "starting sea panel with pid %d", getpid());
    panel_init();
    panel_init_plugins();

    // render loop
    while (running) {
        // wait for render signal
        pthread_mutex_lock(&render_signal.mutex);
        pthread_cond_wait(&render_signal.cond, &render_signal.mutex);

        // render panel
        panel_render();

        pthread_mutex_unlock(&render_signal.mutex);
    }

    logger_log(LOG_SUCCESS, "panel stopped");
    return 0;
}
