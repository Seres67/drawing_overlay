#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    bool mouse_left;
    bool mouse_right;
} InputState;

typedef struct
{
    float x;
    float y;
} MousePostion;

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
    InputState inputs;
    MousePostion mouse;
    Lines lines;
} App;

enum MouseButton
{
    MouseLeft = 1,
    MouseMiddle,
    MouseRight,
    MouseButton4,
    MouseButton5
};

int main(int argc, char *argv[])
{
    App app = {NULL, NULL, true, {false, false}, {0, 0}, {0}};
    app.running = true;
    app.lines.count = 0;
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        SDL_Log("couldn't init SDL: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    if (!SDL_CreateWindowAndRenderer("", 1920, 1080, SDL_WINDOW_TRANSPARENT, &app.window, &app.renderer)) {
        SDL_Log("couldn't create window or renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_Event event;
    while (app.running) {
        while (SDL_PollEvent(&event)) {
            // TODO: handle events
            if (event.type == SDL_EVENT_QUIT)
                app.running = false;
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == MouseLeft) { // left button
                    app.inputs.mouse_left = true;
                    app.mouse.x = event.motion.x;
                    app.mouse.y = event.motion.y;
                }
                if (event.button.button == MouseRight) // right button
                    app.inputs.mouse_right = true;
            }
            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (event.button.button == MouseLeft) // left button
                    app.inputs.mouse_left = false;
                if (event.button.button == MouseRight) // right button
                    app.inputs.mouse_right = false;
            }
            if (app.inputs.mouse_left && event.type == SDL_EVENT_MOUSE_MOTION) {
                app.lines.lines[app.lines.count] =
                    (SDL_FRect){app.mouse.x, app.mouse.y, event.motion.xrel == 0 ? 1 : event.motion.xrel,
                                event.motion.yrel == 0 ? 1 : event.motion.yrel};
                app.lines.count++;
                app.mouse.x += event.motion.xrel;
                app.mouse.y += event.motion.yrel;
                printf("%d\n", app.lines.count);
            }
        }
        // TODO: logic

        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 0);
        SDL_RenderClear(app.renderer);
        SDL_SetRenderDrawColor(app.renderer, 255, 0, 0, 255);
        for (int i = 0; i < app.lines.count; ++i)
            SDL_RenderFillRect(app.renderer, &app.lines.lines[i]);

        SDL_RenderPresent(app.renderer);
    }
    return 0;
}
