#include "config.h"
#include "args.h"

Config config;

void config_init(int argc, char** argv)
{
    set_option('l', "log", "set log file path", true);
    set_option('s', "stdout", "log to stdout", false);
    parse_args(argc, argv);

    // set config values
    const char* log_file = get_long_option("log")->value;
    config.log_file = log_file ? log_file : "/tmp/panel.log";
    config.log_to_stdout = get_long_option("log-stdout")->is_set;
}

void config_free(void)
{
    free_args();
}
