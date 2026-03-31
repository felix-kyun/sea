#include "gstreamer.h"
#include <gst/app/gstappsrc.h>
#include <stdio.h>

typedef struct _gst_data {
    GstElement *pipeline;

    // Elements
    GstElement *appsrc;
    GstElement *convert;
    GstElement *sink;
} gst_data;

gst_data data     = { 0 };
bool     prepared = false;

void
ex_gst_init(int *argc, char ***argv)
{
    gst_init(argc, argv);
    data.pipeline = gst_pipeline_new("sea-extend-gst-pipeline");
    data.appsrc   = gst_element_factory_make("appsrc", "src");
    data.convert  = gst_element_factory_make("videoconvert", "convert");
    data.sink     = gst_element_factory_make("autovideosink", "sink");

    gst_bin_add_many(GST_BIN(data.pipeline), data.appsrc, data.convert, data.sink, NULL);
    gst_element_link_many(data.appsrc, data.convert, data.sink, NULL);
}

void
ex_gst_prepare(struct gst_prepare_data config)
{
    if (prepared)
        return;

    char cap_buffer[1024];
    snprintf(
        cap_buffer,
        sizeof(cap_buffer),
        "video/x-raw,format=%s,width=%d,height=%d,framerate=(fraction)%d/%d",
        config.format,
        config.width,
        config.height,
        config.framerate_num,
        config.framerate_den);
    g_debug("caps: %s", cap_buffer);

    GstCaps *caps = gst_caps_from_string(cap_buffer);
    g_object_set(data.appsrc, "caps", caps, "format", GST_FORMAT_TIME, "is-live", TRUE, "do-timestamp", TRUE, NULL);
    gst_caps_unref(caps);
    gst_element_set_state(data.pipeline, GST_STATE_PLAYING);

    prepared = true;
}

void
ex_gst_push_buffer(GstBuffer *buffer)
{
    gst_app_src_push_buffer(GST_APP_SRC(data.appsrc), buffer);
}

void
ex_gst_free(void)
{
}
