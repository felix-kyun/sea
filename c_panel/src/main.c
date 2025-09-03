#include "log.h"
#include "panel.h"
#include "plugins/date_time.h"

int main(void)
{
    StartRoutine enabled_plugins[] = {
        plugin_date_time
    };

    logger_init("panel.log", false);
    logger_log(LOG_SUCCESS, "starting sea panel");

    panel_init_plugins(enabled_plugins, sizeof(enabled_plugins) / sizeof(enabled_plugins[0]));

    // render loop
    while (1) {
        pthread_mutex_lock(&render_signal.mutex);
        pthread_cond_wait(&render_signal.cond, &render_signal.mutex);

        logger_log(LOG_DEBUG, "rendering panel");
        panel_render();
        logger_log(LOG_DEBUG, "panel rendered");

        pthread_mutex_unlock(&render_signal.mutex);
    }

    logger_log(LOG_INFO, "stopping panel");
    logger_log(LOG_SUCCESS, "panel stopped");
    return 0;
}
