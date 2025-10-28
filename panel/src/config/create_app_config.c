#include "args.h"
#include "config/config.h"
#include <string.h>
#include <unistd.h>

Config config;

void create_base_config(void)
{
    config.log_file = "/tmp/panel.log";
    config.log_to_stdout = false;
}

void create_app_config(int argc, char** argv)
{
    set_option('c', "config", "set config file path", true);
    set_option('l', "log", "set log file path", true);
    set_option('s', "stdout", "log to stdout", false);
    parse_args(argc, argv);

    config_init();
    create_base_config();

    if (get_long_option("config")->is_set) {
        const char* config_path = get_long_option("config")->value;
        config_parse(config_path);
    } else if (access(CONFIG_FILE, F_OK) != -1) {
        config_parse(CONFIG_FILE);
    }

    // set from config
    if (config_get("panel.log_file"))
        config.log_file = config_get("panel.log_file");

    if (config_get("panel.log_to_stdout")) {
        if (strcmp(config_get("panel.log_to_stdout"), "true") == 0)
            config.log_to_stdout = true;
        else
            config.log_to_stdout = false;
    }

    // override from args
    config.log_to_stdout = get_long_option("stdout")->is_set || config.log_to_stdout;
    config.log_file = get_long_option("log")->value ? get_long_option("log")->value : config.log_file;
}
