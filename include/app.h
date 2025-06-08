#ifndef DRAWING_OVERLAY_APP_H_
#define DRAWING_OVERLAY_APP_H_

#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <mouse.h>

typedef struct
{

    int count;
    SDL_FRect lines[100000];
} Lines;

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool running;
    MouseState mouse_state;
    Lines lines;
    TTF_Font *font;
} App;

#endif // DRAWING_OVERLAY_APP_H_
