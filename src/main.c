#include "button.h"
#include "mouse.h"
#include <SDL3/SDL.h>
#include <app.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void reset_screen(App *app) { memset(&app->lines.lines, 0, 100000 * sizeof(SDL_FRect)); };

int main(int argc, char *argv[])
{
    App app = {NULL, NULL, true, {false, false, 0, 0}, {0}};
    Button reset = {"Clear", {100, 100, 40, 40}, NULL, reset_screen};

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        SDL_Log("couldn't init SDL: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!TTF_Init()) {
        SDL_Log("couldn't init TTF: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    app.font = TTF_OpenFont("../assets/font.ttf", 13.f);
    if (!app.font) {
        SDL_Log("couldn't open font: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("", 1920, 1080, SDL_WINDOW_TRANSPARENT | SDL_WINDOW_BORDERLESS, &app.window,
                                     &app.renderer)) {
        SDL_Log("couldn't create window or renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_Color color = {255, 255, 255, SDL_ALPHA_OPAQUE};
    SDL_Surface *text = TTF_RenderText_Blended(app.font, reset.text, 5, color);
    if (text) {
        reset.texture = SDL_CreateTextureFromSurface(app.renderer, text);
        SDL_DestroySurface(text);
    }
    if (!reset.texture) {
        SDL_Log("couldn't create texture: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_Event event;
    while (app.running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                app.running = false;
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == MouseLeft) {
                    if (is_button_clicked(reset, event.button.x, event.button.y)) {
                        reset.callback(&app);
                    }
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

        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(app.renderer, &reset.rect);

        SDL_SetRenderDrawColor(app.renderer, 255, 0, 0, 255);

        SDL_RenderTexture(app.renderer, reset.texture, NULL, &reset.rect);

        for (int i = 0; i < app.lines.count; ++i)
            SDL_RenderFillRect(app.renderer, &app.lines.lines[i]);

        SDL_RenderPresent(app.renderer);
    }
    return 0;
}
