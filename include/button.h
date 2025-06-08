#ifndef DRAWING_OVERLAY_BUTTON_H_
#define DRAWING_OVERLAY_BUTTON_H_

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <app.h>
#include <stdbool.h>

typedef struct
{
    char *text;
    SDL_FRect rect;
    SDL_Texture *texture;
    void (*callback)(App *);
} Button;

bool is_button_clicked(Button button, float x, float y);

#endif // DRAWING_OVERLAY_BUTTON_H_
