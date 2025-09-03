#include "log.h"

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    logger_init("app.log", true);
    logger_log(LOG_INFO, "starting sea panel");

    logger_log(LOG_INFO, "stopping sea panel");
    return 0;
}
