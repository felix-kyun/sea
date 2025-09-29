#pragma once

typedef enum MouseEventType {
    MOUSE_NONE = 0,
    MOUSE_LEFT_CLICK,
    MOUSE_RIGHT_CLICK,
    MOUSE_MIDDLE_CLICK,
    MOUSE_SCROLL_UP,
    MOUSE_SCROLL_DOWN
} MouseEventType;

typedef struct MouseEvent {
    MouseEventType type;
    int x;
} MouseEvent;

void mouse_init(void);
void mouse_free(void);
