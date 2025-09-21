#include "string-utf8.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    // can create empty strings
    string* empty_str = string_init();
    assert(empty_str->byte_length == 0);
    assert(empty_str->char_length == 0);
    string_free(empty_str);

    // can create new strings with utf-8 characters
    string* str = string_new(u8"Hello 😀");
    assert(str->byte_length == 10);
    assert(str->char_length == 7);
    string_free(str);

    // can get length of utf-8 strings
    size_t len = string_length(u8"Hello 😀");
    assert(len == 7);

    // can cast string to c-style string
    string* str_cast = string_new(u8"Hello 😀");
    char* cstr_cast = string_cast(str_cast);
    assert(strcmp(cstr_cast, u8"Hello 😀") == 0);
    assert(memcmp(cstr_cast, str_cast->data, str_cast->byte_length) == 0);
    free(cstr_cast);
    string_free(str_cast);

    // can concatenate utf-8 strings
    string* str1 = string_new(u8"Hello ");
    string* str2 = string_new(u8"😀");
    string* str3 = string_concat(str1, str2);
    assert(str3->byte_length == 10);
    assert(str3->char_length == 7);
    string_free(str1);
    string_free(str2);
    string_free(str3);

    // can set string content from cstring
    // also test if it avoids mallocing if new data fits
    string* str_set = string_new(u8"Hello");
    string_set_cstr(str_set, u8"Hello World");
    assert(str_set->byte_length == 11);
    assert(str_set->char_length == 11);

    // check if it avoids reallocating when new data fits
    char* old_data_ptr = str_set->data;
    string_set_cstr(str_set, u8"Hi");
    assert(str_set->data == old_data_ptr);
    string_free(str_set);

    // can set string content from another string
    string* str_set2 = string_new(u8"Hello");
    string* new_data = string_new(u8"Hello 😀");
    string_set(str_set2, new_data);
    assert(str_set2->byte_length == 10);
    assert(str_set2->char_length == 7);
    // check if it avoids reallocating when new data fits
    old_data_ptr = str_set2->data;
    string_set(str_set2, new_data);
    assert(str_set2->data == old_data_ptr);
    string_free(str_set2);
    string_free(new_data);

    // can compare strings
    string* cmp_str1 = string_new(u8"Hello 😀");
    string* cmp_str2 = string_new(u8"Hello 😀");
    string* cmp_str3 = string_new(u8"Hello World");
    assert(string_equals(cmp_str1, cmp_str2));
    assert(!string_equals(cmp_str1, cmp_str3));
    string_free(cmp_str1);
    string_free(cmp_str2);
    string_free(cmp_str3);

    // can compare string with c-style string
    string* cmp_cstr = string_new(u8"Hello 😀");
    assert(string_equals_cstr(cmp_cstr, u8"Hello 😀"));
    assert(!string_equals_cstr(cmp_cstr, u8"Hello World"));
    string_free(cmp_cstr);

    // can create ascii-only strings
    string* ascii_str = string_new(u8"Hello 😀");
    assert(ascii_str->byte_length == 10);
    assert(ascii_str->char_length == 7);
    string_set_cstr_ascii(ascii_str, u8"Hello 😀");
    assert(ascii_str->byte_length == 6);
    assert(ascii_str->char_length == 6);
    string_free(ascii_str);

    return 0;
}
