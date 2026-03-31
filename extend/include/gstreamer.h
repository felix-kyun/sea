#pragma once
#include <gst/gstbuffer.h>

struct gst_prepare_data {
    char *format;
    int   width;
    int   height;
    int   framerate_num;
    int   framerate_den;
};

void ex_gst_init(int *argc, char ***argv);
void ex_gst_prepare(struct gst_prepare_data data);
void ex_gst_push_buffer(GstBuffer *buffer);
void ex_gst_free(void);
