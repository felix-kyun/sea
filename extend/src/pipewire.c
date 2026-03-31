#include "pipewire.h"
#include <drm_fourcc.h>
#include <glib-unix.h>
#include <glib.h>
#include <pipewire/context.h>
#include <pipewire/core.h>
#include <pipewire/keys.h>
#include <pipewire/log.h>
#include <pipewire/loop.h>
#include <pipewire/port.h>
#include <pipewire/properties.h>
#include <pipewire/stream.h>
#include <spa/buffer/buffer.h>
#include <spa/debug/format.h>
#include <spa/debug/pod.h>
#include <spa/debug/types.h>
#include <spa/param/buffers.h>
#include <spa/param/format.h>
#include <spa/param/param.h>
#include <spa/param/video/format-utils.h>
#include <spa/param/video/format.h>
#include <spa/pod/builder.h>
#include <spa/pod/vararg.h>
#include <spa/support/log.h>
#include <spa/utils/type.h>
#include <stddef.h>

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
    int                   pw_fd;

    struct spa_video_info format;

    // glib
    GSource *source;
} pw_data;

static pw_data      data         = { 0 };
static GSourceFuncs source_funcs = {
    .dispatch = pw_dispatch_handler,
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
    g_source_add_unix_fd(data.source, data.pw_fd, G_IO_IN | G_IO_ERR);
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
        SPA_POD_CHOICE_ENUM_Long(2, DRM_FORMAT_MOD_INVALID, DRM_FORMAT_MOD_INVALID));

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
        PW_STREAM_FLAG_AUTOCONNECT | PW_STREAM_FLAG_MAP_BUFFERS | PW_STREAM_FLAG_RT_PROCESS,
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
pw_dispatch_handler(
    [[maybe_unused]] GSource *source, [[maybe_unused]] GSourceFunc callback, [[maybe_unused]] gpointer user_data)
{
    g_debug("pw_dispatch_handler invoked");
    int result = pw_loop_iterate(data.loop, 0);
    if (result < 0) {
        g_debug("pw_dispatch_handler: pw_loop_iterate failed: %d", result);
        return false;
    }
    return true;
}

void
on_process(void *user_data)
{
    (void)user_data;
    g_debug("on_process invoked");
}
void
on_param_changed(void *userdata, uint32_t id, const struct spa_pod *param)
{
    (void)userdata;
    (void)id;

    g_debug("param_changed");
    if (param == nullptr) {
        g_debug("param is null, returning");
        return;
    }
    if (pw_log_level_enabled(SPA_LOG_LEVEL_INFO)) {
        spa_debug_pod(4, NULL, param);
    }
}
