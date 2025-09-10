#pragma once

typedef void (*input_callback)(const char* input);

void input_reader_init(input_callback handler);
void input_reader_free(void);
