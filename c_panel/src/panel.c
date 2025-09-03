#include "panel.h"
#include "string-utf8.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    string* str = string_new(u8"Hello 😀");
    printf("%s\n", string_cast(str));

    string_free(str);
    return 0;
}
