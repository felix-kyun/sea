#include "args.h"
#include "config/config.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Config config;

void create_base_config(void)
{
    config.log_file = "/tmp/panel.log";
    config.config_file = CONFIG_FILE;
    config.log_to_stdout = false;
    config.module_spacing = 1;
    config.left_modules = create_string_array();
    config.center_modules = create_string_array();
    config.right_modules = create_string_array();
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
        config.config_file = get_long_option("config")->value;
    }

    if (access(config.config_file, F_OK) == -1) {
        // config file does not exist
        logger_log(LOG_WARN, "config file %s does not exist, using default config", config.config_file);
        exit(1);
    }

    config_parse(config.config_file);

    // set from config
    if (config_get("panel", "log_file"))
        config.log_file = config_get("panel", "log_file");

    if (config_get("panel", "log_to_stdout")) {
        if (strcmp(config_get("panel", "log_to_stdout"), "true") == 0)
            config.log_to_stdout = true;
        else
            config.log_to_stdout = false;
    }

    if (config_get("panel", "module_spacing")) {
        config.module_spacing = atoi(config_get("panel", "module_spacing"));
    }

    if (config_get("panel", "left_modules")) {
        string_array_from_csv(config.left_modules, config_get("panel", "left_modules"));
    }
    if (config_get("panel", "center_modules")) {
        string_array_from_csv(config.center_modules, config_get("panel", "center_modules"));
    }
    if (config_get("panel", "right_modules")) {
        string_array_from_csv(config.right_modules, config_get("panel", "right_modules"));
    }

    // override from args
    config.log_to_stdout = get_long_option("stdout")->is_set || config.log_to_stdout;
    config.log_file = get_long_option("log")->value ? get_long_option("log")->value : config.log_file;
}

void destroy_app_config(void)
{
    free_string_array(config.left_modules);
    free_string_array(config.center_modules);
    free_string_array(config.right_modules);

    config_free();
}
