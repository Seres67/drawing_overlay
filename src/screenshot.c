#include <SDL3_image/SDL_image.h>
#include <screenshot.h>

const void *clipboard_data_cb(void *userdata, const char *mime_type, size_t *size)
{
    ClipboardImage *clip = (ClipboardImage *)userdata;
    *size = clip->size;
    return clip->data;
}

void clipboard_cleanup_cb(void *userdata)
{
    ClipboardImage *clip = (ClipboardImage *)userdata;
    if (clip) {
        if (clip->data)
            free(clip->data);
        free(clip);
    }
}

SDL_Surface *grab_screenshot_surface()
{
    const char *tmpfile = "/tmp/screenshot.png";
    int ret = system("grim /tmp/screenshot.png");
    if (ret != 0) {
        SDL_Log("grim screenshot failed");
        return NULL;
    }
    SDL_Surface *surface = IMG_Load(tmpfile);
    if (!surface) {
        SDL_Log("Failed to load screenshot image: %s", SDL_GetError());
        return NULL;
    }
    return surface;
}

ClipboardImage *surface_to_png_data(SDL_Surface *surface)
{
    ClipboardImage *clip = malloc(sizeof(ClipboardImage));
    if (!clip)
        return NULL;

    size_t initial_size = 1024 * 1024;

    void *buffer = malloc(initial_size);
    if (!buffer) {
        free(clip);
        return NULL;
    }

    SDL_IOStream *io = SDL_IOFromMem(buffer, initial_size);
    if (!io) {
        free(buffer);
        free(clip);
        return NULL;
    }

    if (!IMG_SavePNG_IO(surface, io, 0)) {
        SDL_CloseIO(io);
        free(buffer);
        free(clip);
        return NULL;
    }

    size_t actual_size = SDL_TellIO(io);
    void *final_buffer = malloc(actual_size);
    if (!final_buffer) {
        SDL_CloseIO(io);
        free(buffer);
        free(clip);
        return NULL;
    }

    SDL_SeekIO(io, 0, SDL_IO_SEEK_SET);
    SDL_ReadIO(io, final_buffer, actual_size);
    SDL_CloseIO(io);
    free(buffer);

    clip->data = final_buffer;
    clip->size = actual_size;
    return clip;
}
