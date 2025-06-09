#ifndef DRAWING_OVERLAY_APP_H_
#define DRAWING_OVERLAY_APP_H_

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <mouse.h>
#include <stdbool.h>

#define POINTS_COUNT 2500
#define STROKES_COUNT 200

typedef struct
{
    int count;
    SDL_FPoint points[POINTS_COUNT];
    SDL_Color color;
    bool hidden;
} Stroke;

typedef struct
{
    int count;
    Stroke strokes[STROKES_COUNT];
} Canvas;

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool running;
    MouseState mouse_state;
    Canvas canvas;
    TTF_Font *font;
    SDL_Color current_color;
} App;

// TODO: changes lines to strokes
// each stroke is an array of rects
// store array of strokes
// when ctrl+z hide last troke
// if drawing while last stroke is hidden, replace it
// ctrl+y unhide last stroke

#endif // DRAWING_OVERLAY_APP_H_
