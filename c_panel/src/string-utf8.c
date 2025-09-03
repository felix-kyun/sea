#include "string-utf8.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

string* string_new(const char* data)
{
    string* str = (string*)malloc(sizeof(string));
    str->byte_length = strlen(data);

    // actual utf-8 string length
    str->char_length = string_length(data);

    // strlen on a utf-8 string returns the number of bytes
    str->data = (char*)malloc(str->byte_length * sizeof(char));
    memcpy(str->data, data, str->byte_length);

    return str;
}

size_t string_length(const char* str)
{
    size_t length = 0;
    while (*str) {
        if ((*str & 0xc0) != 0x80) {
            length++;
        }
        str++;
    }

    return length;
}

void string_free(string* str)
{
    free(str->data);
    free(str);
}

char* string_cast(string* str)
{
    char* cstr = (char*)malloc((str->byte_length + 1) * sizeof(char));

    memcpy(cstr, str->data, str->byte_length);
    cstr[str->byte_length] = '\0';

    return cstr;
}

string* string_concat(string* str1, string* str2)
{
    string* new_string = (string*)malloc(sizeof(string));

    new_string->byte_length = str1->byte_length + str2->byte_length;
    new_string->char_length = str1->char_length + str2->char_length;

    new_string->data = (char*)malloc(new_string->byte_length * sizeof(char));
    memcpy(new_string->data, str1->data, str1->byte_length);
    memcpy(new_string->data + str1->byte_length, str2->data, str2->byte_length);

    return new_string;
}
