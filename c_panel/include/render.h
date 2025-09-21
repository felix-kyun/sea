#pragma once

typedef struct RenderInfo {
    int left_padding;
    int right_padding;
} RenderInfo;

extern RenderInfo render_info;

void render_init(void);
void render_recalculate_size(void);
