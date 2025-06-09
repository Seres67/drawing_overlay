#ifndef DRAWING_OVERLAY_BUTTON_H_
#define DRAWING_OVERLAY_BUTTON_H_

#include <SDL3/SDL_pixels.h>
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

typedef struct ColorButton_s
{
    SDL_Color color;
    SDL_FRect rect;
    SDL_Texture *texture;
    void (*callback)(App *, struct ColorButton_s);
} ColorButton;

bool is_mouse_in_button(Button button, float x, float y);
void button_draw(Button button, App app);

bool is_mouse_in_color_button(ColorButton button, float x, float y);
void button_color_draw(ColorButton button, App *app);

#endif // DRAWING_OVERLAY_BUTTON_H_
