#include "args.h"
#include <assert.h>
#include <string.h>

int main(void)
{
    // creation
    set_option('h', "help", "display this help message", false);
    set_option('V', "verbose", "enable verbose logging", false);
    set_option('o', "output", "set output file", true);
    set_argument("input", "input file", true);

    const int argc = 4;
    char* argv[] = {
        "program",
        "-hVo",
        "output.txt",
        "input.txt",
    };

    parse_args(argc, argv);

    // test
    assert(get_short_option('h')->is_set == true);
    assert(get_long_option("help")->is_set == true);
    assert(get_short_option('V')->is_set == true);
    assert(get_long_option("verbose")->is_set == true);
    assert(get_short_option('o')->is_set == true);
    assert(get_long_option("output")->is_set == true);
    assert(get_short_option('o')->value != NULL);
    assert(get_long_option("output")->value != NULL);
    assert(strcmp(get_short_option('o')->value, "output.txt") == 0);
    assert(strcmp(get_long_option("output")->value, "output.txt") == 0);
    assert(get_argument("input")->value != NULL);
    assert(strcmp(get_argument("input")->value, "input.txt") == 0);

    free_args();
}
