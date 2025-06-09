#ifndef DRAWING_OVERLAY_SCREENSHOT_H_
#define DRAWING_OVERLAY_SCREENSHOT_H_

#include <app.h>
#include <stdlib.h>

typedef struct
{
    void *data;
    size_t size;
} ClipboardImage;

const void *clipboard_data_cb(void *userdata, const char *mime_type, size_t *size);
void clipboard_cleanup_cb(void *userdata);
SDL_Surface *grab_screenshot_surface();
ClipboardImage *surface_to_png_data(SDL_Surface *surface);

#endif // DRAWING_OVERLAY_SCREENSHOT_H_
