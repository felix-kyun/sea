#pragma once
#include <stdbool.h>
#include <stddef.h>

struct string {
    // pointer to a non null terminated utf-8 capable string
    char* data;

    // length in bytes
    // requires as the string is not null terminated
    size_t byte_length;

    // length in utf-8 characters
    size_t char_length;
};

typedef struct string string;

// @return a pointer to a newly allocated empty utf-8 string struct
// don't forget to free the string with string_free
string* string_init(void);

// @param data: a c-style string (null-terminated)
// @return a pointer to a newly allocated utf-8 string struct
// don't forget to free the string with string_free
string* string_new(const char* data);

// @param str: a c-style string (null-terminated) containing utf-8 data
// @return the number of utf-8 characters in the string
// this is different from strlen, which returns the number of bytes
size_t string_length(const char* str);

// @param str: a pointer to a utf-8 string struct
// Frees the memory allocated for the string struct and its data
void string_free(string* str);

// @param str: a pointer to a utf-8 string struct
// @return the underlying c-style string (null-terminated)
char* string_cast(string* str);

// @param str1: first string to concatenate
// @param str2: second string to concatenate
// @return a new string which is the concatenation of str1 and str2
string* string_concat(string* str1, string* str2);

// @param str: a pointer to a utf-8 string struct
// @param data: a pointer to a utf-8 string struct
// Sets the content of the string to the given data
// prevents unnecessary allocations if the new data fits in the existing allocation
void string_set(string* str, string* data);

// @param str: a pointer to a utf-8 string struct
// @param data: a c-style string (null-terminated) containing utf-8 data
// Sets the content of the string to the given data
// prevents unnecessary allocations if the new data fits in the existing allocation
void string_set_cstr(string* str, const char* data);

// @param str1: first string to compare
// @param str2: second string to compare
// @return true if the strings are equal, false otherwise
bool string_equals(string* str1, string* str2);

// @param str1: first string to compare
// @param str2: second c-style string (null-terminated) to compare
// @return true if the strings are equal, false otherwise
bool string_equals_cstr(string* str1, const char* str2);
