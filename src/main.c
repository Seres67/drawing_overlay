#include "button.h"
#include "mouse.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <app.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void reset_screen(App *app)
{
    for (int i = 0; i < app->strokes.count; ++i)
        memset(app->strokes.strokes[i].points, 0, POINTS_COUNT * sizeof(SDL_FRect));
    memset(&app->strokes.strokes, 0, STROKES_COUNT * sizeof(Stroke));
    app->strokes.count = 0;
};

void undo_stroke(App *app)
{
    if (app->strokes.count > 0) {
        app->strokes.strokes[app->strokes.count - 1].hidden = true;
        app->strokes.count--;
    }
}

void redo_stroke(App *app)
{
    if (app->strokes.strokes[app->strokes.count].hidden) {
        app->strokes.strokes[app->strokes.count].hidden = false;
        app->strokes.count++;
    }
}

void save_to_file(App *app) {}

int main(int argc, char *argv[])
{
    App app = {NULL, NULL, true, {false, false, 0, 0}, {0}, NULL};
    Button clear = {"Clear", {0, 0, 40, 40}, NULL, reset_screen};
    Button undo = {"Undo", {40, 0, 40, 40}, NULL, undo_stroke};
    Button redo = {"Redo", {80, 0, 40, 40}, NULL, redo_stroke};
    Button save = {"Save", {120, 0, 40, 40}, NULL, save_to_file};
    Button buttons[] = {clear, undo, redo, save, {NULL}};

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
    for (int i = 0; buttons[i].text; ++i) {
        SDL_Surface *text = TTF_RenderText_Blended(app.font, buttons[i].text, 5, color);
        if (text) {
            buttons[i].texture = SDL_CreateTextureFromSurface(app.renderer, text);
            SDL_DestroySurface(text);
        }
        if (!buttons[i].texture) {
            SDL_Log("couldn't create texture: %s\n", SDL_GetError());
            return SDL_APP_FAILURE;
        }
    }

    SDL_Event event;
    while (app.running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                app.running = false;
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == MouseLeft) {
                    app.mouse_state.mouse_left = true;
                    bool found_button = false;
                    for (int i = 0; buttons[i].text; ++i) {
                        if (is_button_clicked(buttons[i], event.button.x, event.button.y)) {
                            buttons[i].callback(&app);
                            found_button = true;
                        }
                    }
                    if (found_button)
                        break;
                    app.mouse_state.x = event.motion.x;
                    app.mouse_state.y = event.motion.y;
                    if (app.strokes.strokes[app.strokes.count].hidden) {
                        app.strokes.strokes[app.strokes.count].hidden = false;
                        memset(app.strokes.strokes[app.strokes.count].points, 0, POINTS_COUNT * sizeof(SDL_FPoint));
                        app.strokes.strokes[app.strokes.count].count = 0;
                        // TODO: also memset 0 all following strokes and points and reset count and hidden
                    }
                    app.strokes.count++;
                }
                if (event.button.button == MouseRight)
                    app.mouse_state.mouse_right = true;
            }
            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (event.button.button == MouseLeft) {
                    app.mouse_state.mouse_left = false;
                }
                if (event.button.button == MouseRight)
                    app.mouse_state.mouse_right = false;
            }
            if (!app.mouse_state.mouse_left && event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.mod == SDL_KMOD_LCTRL) {
                    if (event.key.key == SDLK_Z) {
                        undo_stroke(&app);
                    } else if (event.key.key == SDLK_Y) {
                        redo_stroke(&app);
                    }
                }
            }
            if (app.mouse_state.mouse_left && event.type == SDL_EVENT_MOUSE_MOTION) {
                app.strokes.strokes[app.strokes.count - 1].points[app.strokes.strokes[app.strokes.count - 1].count] =
                    (SDL_FPoint){app.mouse_state.x, app.mouse_state.y};
                app.strokes.strokes[app.strokes.count - 1].count++;
                app.mouse_state.x += event.motion.xrel;
                app.mouse_state.y += event.motion.yrel;
            }
        }
        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 0);
        SDL_RenderClear(app.renderer);

        // TODO: draw corners

        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
        for (int i = 0; buttons[i].text; ++i)
            SDL_RenderFillRect(app.renderer, &buttons[i].rect);

        SDL_SetRenderDrawColor(app.renderer, 255, 0, 0, 255);

        for (int i = 0; buttons[i].text; ++i)
            SDL_RenderTexture(app.renderer, buttons[i].texture, NULL, &buttons[i].rect);

        for (int i = 0; i < app.strokes.count; ++i)
            if (!app.strokes.strokes[i].hidden)
                SDL_RenderLines(app.renderer, app.strokes.strokes[i].points, app.strokes.strokes[i].count);
        SDL_RenderPresent(app.renderer);
    }
    return 0;
}
