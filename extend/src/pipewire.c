#include "pipewire.h"
#include "gstreamer.h"
#include "utils.h"
#include <drm_fourcc.h>
#include <spa/debug/pod.h>
#include <spa/param/video/format-utils.h>
#include <sys/mman.h>

// TODO: reduce tight coupling

typedef struct _pw_data {
    // acquired from portal
    guint32 node_id;
    gint    source_fd;

    // pw
    struct pw_loop       *loop;
    struct pw_stream     *stream;
    struct pw_context    *context;
    struct pw_core       *core;
    struct pw_properties *props;
    void                 *tag;
    int                   pw_fd;

    struct spa_video_info format;

    // glib
    GSource *source;
} pw_data;

static pw_data      data         = { 0 };
static GSourceFuncs source_funcs = {
    .dispatch = pw_handle_dispatch,
};
static struct pw_stream_events stream_events = {
    PW_VERSION_STREAM_EVENTS,
    .param_changed = on_param_changed,
    .process       = on_process,
};

struct pw_loop *
pw_create_loop()
{
    data.loop = pw_loop_new(nullptr);
    return data.loop;
}

void
pw_setup(guint32 node, gint fd)
{
    g_debug("pw_setup invoked");

    data.node_id   = node;
    data.source_fd = fd;
    data.context   = pw_context_new(data.loop, nullptr, 0);
    data.core      = pw_context_connect_fd(data.context, data.source_fd, nullptr, 0);
    data.pw_fd     = pw_loop_get_fd(data.loop);

    // glib source
    data.source = g_source_new(&source_funcs, sizeof(GSource));
    data.tag    = g_source_add_unix_fd(data.source, data.pw_fd, G_IO_IN | G_IO_ERR | G_IO_HUP);
    g_source_attach(data.source, nullptr);
    g_source_unref(data.source);

    data.props = pw_properties_new(
        PW_KEY_MEDIA_TYPE, "Video", PW_KEY_MEDIA_CATEGORY, "Capture", PW_KEY_MEDIA_ROLE, "Screen", nullptr);
    data.stream = pw_stream_new_simple(data.loop, "sea-extend", data.props, &stream_events, &data);

    const struct spa_pod  *params[2];
    uint8_t                buffer[1024];
    struct spa_pod_builder builder = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
    params[0]                      = spa_pod_builder_add_object(
        &builder,
        SPA_TYPE_OBJECT_Format,
        SPA_PARAM_EnumFormat,

        // media type
        SPA_FORMAT_mediaType,
        SPA_POD_Id(SPA_MEDIA_TYPE_video),
        SPA_FORMAT_mediaSubtype,
        SPA_POD_Id(SPA_MEDIA_SUBTYPE_raw),

        // format
        SPA_FORMAT_VIDEO_format,
        SPA_POD_CHOICE_ENUM_Id(
            4, SPA_VIDEO_FORMAT_NV12, SPA_VIDEO_FORMAT_RGBx, SPA_VIDEO_FORMAT_BGRx, SPA_VIDEO_FORMAT_I420),

        // size
        SPA_FORMAT_VIDEO_size,
        SPA_POD_CHOICE_RANGE_Rectangle(&SPA_RECTANGLE(320, 240), &SPA_RECTANGLE(1, 1), &SPA_RECTANGLE(4096, 4096)),

        // framerate
        SPA_FORMAT_VIDEO_framerate,
        SPA_POD_CHOICE_RANGE_Fraction(&SPA_FRACTION(60, 1), &SPA_FRACTION(0, 1), &SPA_FRACTION(1000, 1)),

        // modifier
        SPA_FORMAT_VIDEO_modifier,
        SPA_POD_CHOICE_ENUM_Long(2, DRM_FORMAT_MOD_LINEAR, DRM_FORMAT_MOD_LINEAR));

    // request dma with fallback
    params[1] = spa_pod_builder_add_object(
        &builder,
        SPA_TYPE_OBJECT_ParamBuffers,
        SPA_PARAM_Buffers,

        // buffer count
        SPA_PARAM_BUFFERS_buffers,
        SPA_POD_CHOICE_RANGE_Int(16, 2, 16),

        // buffer type
        SPA_PARAM_BUFFERS_dataType,
        SPA_POD_CHOICE_FLAGS_Int((1 << SPA_DATA_DmaBuf) | (1 << SPA_DATA_MemFd)));

    pw_stream_connect(
        data.stream,
        PW_DIRECTION_INPUT,
        data.node_id,
        PW_STREAM_FLAG_AUTOCONNECT | PW_STREAM_FLAG_MAP_BUFFERS /* | PW_STREAM_FLAG_RT_PROCESS */,
        params,
        2);
}

void
pw_free()
{
    pw_core_disconnect(data.core);
    pw_context_destroy(data.context);
    pw_loop_destroy(data.loop);
}

gboolean
pw_handle_dispatch(
    [[maybe_unused]] GSource *source, [[maybe_unused]] GSourceFunc callback, [[maybe_unused]] gpointer user_data)
{
    f_tag();

    if (g_source_query_unix_fd(source, data.tag) & (G_IO_ERR | G_IO_HUP)) {
        g_warning("pw error/hangup");
        return G_SOURCE_REMOVE;
    }

    int result = pw_loop_iterate(data.loop, 0);
    if (result < 0) {
        g_debug("pw_loop_iterate failed: %d", result);
        return false;
    }

    return G_SOURCE_CONTINUE;
}

void
on_process([[maybe_unused]] void *user_data)
{
    f_tag();
    struct pw_buffer  *pw_buf;
    struct spa_buffer *spa_buf;
    struct spa_data   *spa_data;

    if ((pw_buf = pw_stream_dequeue_buffer(data.stream)) == nullptr) {
        return;
    }

    spa_buf  = pw_buf->buffer;
    spa_data = &spa_buf->datas[0];
    if (spa_data->type != SPA_DATA_DmaBuf) {
        g_warning("spa_data is not dma-buf, skipping");
        goto enqueue;
    }

    if (pw_log_level_enabled(SPA_LOG_LEVEL_INFO)) {
        debug_spa_data(spa_data);
    }

    uint32_t size = (uint)spa_data->chunk->stride * 1080;
    if (size == 0) {
        g_warning("chunk size is 0, skipping");
        goto enqueue;
    }

    void *ptr = mmap(NULL, size, PROT_READ, MAP_SHARED, (int)spa_data->fd, 0);
    if (ptr == MAP_FAILED) {
        g_warning("mmap failed, skipping");
        goto enqueue;
    }

    unsigned char *p = (unsigned char *)ptr;
    g_debug("first pixel: %u %u %u %u", p[0], p[1], p[2], p[3]);

    GstBuffer *gst_buf = gst_buffer_new_wrapped_full(GST_MEMORY_FLAG_READONLY, ptr, size, 0, size, NULL, NULL);
    ex_gst_push_buffer(gst_buf);

enqueue:
    pw_stream_queue_buffer(data.stream, pw_buf);
}

void
on_param_changed(void *userdata, uint32_t id, const struct spa_pod *param)
{
    (void)userdata;
    (void)id;

    f_tag();
    if (param == nullptr) {
        g_debug("param is null, returning");
        return;
    }
    if (pw_log_level_enabled(SPA_LOG_LEVEL_INFO)) {
        spa_debug_pod(4, NULL, param);
    }
    // check if format caps
    if (id == SPA_PARAM_Format) {
        struct spa_video_info_raw info = { 0 };
        if (spa_format_video_raw_parse(param, &info) < 0) {
            return;
        }

        struct gst_prepare_data config = {
            .format        = (char *)spa_debug_type_find_short_name(spa_type_video_format, info.format),
            .width         = (int)info.size.width,
            .height        = (int)info.size.height,
            .framerate_num = (int)info.framerate.num,
            .framerate_den = (int)info.framerate.denom,
        };

        ex_gst_prepare(config);
    }
}
