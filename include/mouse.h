#ifndef DRAWING_OVERLAY_MOUSE_H_
#define DRAWING_OVERLAY_MOUSE_H_

#include <stdbool.h>

typedef struct
{
    bool mouse_left;
    bool mouse_right;
    float x;
    float y;

} MouseState;

enum MouseButton
{
    MouseLeft = 1,
    MouseMiddle,
    MouseRight,
    MouseButton4,
    MouseButton5
};

#endif // DRAWING_OVERLAY_MOUSE_H_
