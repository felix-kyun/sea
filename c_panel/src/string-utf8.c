#include "string-utf8.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

string* string_init(void)
{
    string* str = (string*)malloc(sizeof(string));
    str->data = NULL;
    str->byte_length = 0;
    str->char_length = 0;
    return str;
}

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

        // skip csi color sequences
        if (*str == '\033' && *(str + 1) == '[') {
            str += 2;

            // go till the final byte of the escape sequence
            while (*str && !(*str == 'm'))
                str++;

            // skip 'm'
            str++;
            continue;
        }

        // count utf-8 codepoints
        if ((*str & 0xc0) != 0x80) {
            length++;
        }
        str++;
    }

    return length;
}

void string_free(string* str)
{
    if (str) {
        if (str->data) {
            free(str->data);
        }
        free(str);
    }
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

    if (str1->data)
        memcpy(new_string->data, str1->data, str1->byte_length);

    if (str2->data)
        memcpy(new_string->data + str1->byte_length, str2->data, str2->byte_length);

    return new_string;
}

void string_set(string* str, string* data)
{
    if (data->byte_length > str->byte_length) {
        if (str->data) {
            free(str->data);
        }
        str->data = (char*)malloc(data->byte_length * sizeof(char));
    }

    memcpy(str->data, data->data, data->byte_length);
    str->byte_length = data->byte_length;
    str->char_length = data->char_length;
}

void string_set_cstr(string* str, const char* data)
{
    size_t new_byte_length = strlen(data);

    if (new_byte_length > str->byte_length) {
        if (str->data) {
            free(str->data);
        }
        str->data = (char*)malloc(new_byte_length * sizeof(char));
    }

    memcpy(str->data, data, new_byte_length);
    str->byte_length = new_byte_length;
    str->char_length = string_length(data);
}

static size_t ascii_count(const char* data)
{
    size_t count = 0;
    size_t len = strlen(data);
    for (size_t i = 0; i < len; i++) {
        if ((unsigned char)data[i] <= 0x7F) {
            count++;
        }
    }
    return count;
}

void string_set_cstr_ascii(string* str, const char* data)
{
    size_t new_byte_length = ascii_count(data);
    if (new_byte_length > str->byte_length) {
        if (str->data) {
            free(str->data);
        }
        str->data = (char*)malloc(new_byte_length * sizeof(char));
    }

    // copy ascii values byte by byte
    for (size_t read = 0, write = 0; write < new_byte_length; read++) {
        if ((unsigned char)data[read] <= 0x7F) {
            str->data[write++] = data[read];
        }
    }

    // ascii only uses one byte per character
    // so byte_length == char_length
    str->byte_length = new_byte_length;
    str->char_length = new_byte_length;
}

bool string_equals(string* str1, string* str2)
{
    if (str1->byte_length != str2->byte_length) {
        return false;
    }

    return memcmp(str1->data, str2->data, str1->byte_length) == 0;
}

bool string_equals_cstr(string* str1, const char* str2)
{
    size_t str2_length = strlen(str2);
    if (str1->byte_length != str2_length) {
        return false;
    }

    return memcmp(str1->data, str2, str1->byte_length) == 0;
}
