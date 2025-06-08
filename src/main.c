#include "mouse.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>
#include <stdlib.h>

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
} App;

int main(int argc, char *argv[])
{
    App app = {NULL, NULL, true, {false, false, 0, 0}, {0}};

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        SDL_Log("couldn't init SDL: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (!SDL_CreateWindowAndRenderer("", 1920, 1080, SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS, &app.window,
                                     &app.renderer)) {
        SDL_Log("couldn't create window or renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_Event event;
    while (app.running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                app.running = false;
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == MouseLeft) {
                    app.mouse_state.mouse_left = true;
                    app.mouse_state.x = event.motion.x;
                    app.mouse_state.y = event.motion.y;
                }
                if (event.button.button == MouseRight)
                    app.mouse_state.mouse_right = true;
            }
            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (event.button.button == MouseLeft)
                    app.mouse_state.mouse_left = false;
                if (event.button.button == MouseRight)
                    app.mouse_state.mouse_right = false;
            }
            if (app.mouse_state.mouse_left && event.type == SDL_EVENT_MOUSE_MOTION) {
                app.lines.lines[app.lines.count] =
                    (SDL_FRect){app.mouse_state.x, app.mouse_state.y, event.motion.xrel == 0 ? 1 : event.motion.xrel,
                                event.motion.yrel == 0 ? 1 : event.motion.yrel};
                app.lines.count++;
                app.mouse_state.x += event.motion.xrel;
                app.mouse_state.y += event.motion.yrel;
            }
        }
        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 0);
        SDL_RenderClear(app.renderer);

        // TODO: draw corners

        SDL_SetRenderDrawColor(app.renderer, 255, 0, 0, 255);
        for (int i = 0; i < app.lines.count; ++i)
            SDL_RenderFillRect(app.renderer, &app.lines.lines[i]);

        SDL_RenderPresent(app.renderer);
    }
    return 0;
}
