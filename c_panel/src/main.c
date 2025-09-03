#include "log.h"

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    logger_init("panel.log", true);
    logger_log(LOG_INFO, "starting sea panel");

    logger_log(LOG_ERROR, "stopping sea panel");
    return 0;
}
