#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <app.h>
#include <button.h>
#include <mouse.h>
#include <screenshot.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

void clear_screen(App *app)
{
    for (int i = 0; i < app->canvas.count; ++i)
        memset(app->canvas.strokes[i].points, 0, POINTS_COUNT * sizeof(SDL_FRect));
    memset(&app->canvas.strokes, 0, STROKES_COUNT * sizeof(Stroke));
    app->canvas.count = 0;
};

void undo_stroke(App *app)
{
    if (app->canvas.count > 0) {
        app->canvas.strokes[app->canvas.count - 1].hidden = true;
        app->canvas.count--;
    }
}

void redo_stroke(App *app)
{
    if (app->canvas.strokes[app->canvas.count].hidden) {
        app->canvas.strokes[app->canvas.count].hidden = false;
        app->canvas.count++;
    }
}

void copy_to_clipboard(App *app)
{
    SDL_Surface *screenshot = grab_screenshot_surface();
    if (!screenshot)
        return;
    ClipboardImage *clip = surface_to_png_data(screenshot);
    SDL_DestroySurface(screenshot);
    if (!clip)
        return;
    static const char *mime_types[] = {"image/png"};
    bool success = SDL_SetClipboardData(clipboard_data_cb, clipboard_cleanup_cb, clip, mime_types, 1);
    if (!success) {
        SDL_Log("Failed to set clipboard data: %s\n", SDL_GetError());
        clipboard_cleanup_cb(clip);
    }
}

void save_to_file(App *app) {}

void set_color(App *app, const ColorButton button) { app->current_color = button.color; }

int main(int argc, char *argv[])
{
    App app = {NULL, NULL, true, {false, false, 0, 0}, {0}, NULL, {255, 0, 0, 255}};
    Button clear = {"Clear", {0, 0, 40, 40}, NULL, clear_screen};
    Button undo = {"Undo", {40, 0, 40, 40}, NULL, undo_stroke};
    Button redo = {"Redo", {80, 0, 40, 40}, NULL, redo_stroke};
    // TODO: check if grim is installed
    Button copy = {"Copy", {120, 0, 40, 40}, NULL, copy_to_clipboard};
    Button save = {"Save", {160, 0, 40, 40}, NULL, save_to_file};
    Button buttons[] = {clear, undo, redo, copy, save, {NULL}};

    ColorButton red = {{255, 0, 0, 255}, {200, 0, 40, 40}, NULL, set_color};
    ColorButton green = {{0, 255, 0, 255}, {240, 0, 40, 40}, NULL, set_color};
    ColorButton blue = {{0, 0, 255, 255}, {280, 0, 40, 40}, NULL, set_color};
    ColorButton yellow = {{255, 255, 0, 255}, {320, 0, 40, 40}, NULL, set_color};
    ColorButton black = {{0, 0, 0, 255}, {360, 0, 40, 40}, NULL, set_color};
    ColorButton white = {{255, 255, 255, 255}, {400, 0, 40, 40}, NULL, set_color};
    ColorButton colors[] = {red, green, blue, yellow, black, white, {0}};

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
                        if (is_mouse_in_button(buttons[i], event.button.x, event.button.y)) {
                            buttons[i].callback(&app);
                            found_button = true;
                        }
                    }
                    if (found_button)
                        break;
                    found_button = false;
                    for (int i = 0; colors[i].color.a != 0; ++i) {
                        if (is_mouse_in_color_button(colors[i], event.button.x, event.button.y)) {
                            colors[i].callback(&app, colors[i]);
                            found_button = true;
                        }
                    }
                    if (found_button)
                        break;
                    app.mouse_state.x = event.motion.x;
                    app.mouse_state.y = event.motion.y;
                    // NOTE: this replaces next stroke if it is hidden
                    if (app.canvas.strokes[app.canvas.count].hidden) {
                        app.canvas.strokes[app.canvas.count].hidden = false;
                        memset(app.canvas.strokes[app.canvas.count].points, 0, POINTS_COUNT * sizeof(SDL_FPoint));
                        app.canvas.strokes[app.canvas.count].count = 0;
                        // TODO: also memset 0 all following strokes and points and reset count and hidden
                    }
                    app.canvas.strokes[app.canvas.count].color = app.current_color;
                    app.canvas.count++;
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
                    } else if (event.key.key == SDLK_R) {
                        clear_screen(&app);
                    }
                }
            }
            if (app.mouse_state.mouse_left && event.type == SDL_EVENT_MOUSE_MOTION) {
                app.canvas.strokes[app.canvas.count - 1].points[app.canvas.strokes[app.canvas.count - 1].count] =
                    (SDL_FPoint){app.mouse_state.x, app.mouse_state.y};
                app.canvas.strokes[app.canvas.count - 1].count++;
                app.mouse_state.x += event.motion.xrel;
                app.mouse_state.y += event.motion.yrel;
            }
        }
        // NOTE: setting window transparent
        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 0);
        SDL_RenderClear(app.renderer);

        for (int i = 0; i < app.canvas.count; ++i) {
            if (!app.canvas.strokes[i].hidden) {
                SDL_SetRenderDrawColor(app.renderer, app.canvas.strokes[i].color.r, app.canvas.strokes[i].color.g,
                                       app.canvas.strokes[i].color.b, app.canvas.strokes[i].color.a);
                SDL_RenderLines(app.renderer, app.canvas.strokes[i].points, app.canvas.strokes[i].count);
            }
        }

        for (int i = 0; buttons[i].text; ++i)
            button_draw(buttons[i], app);
        for (int i = 0; colors[i].color.a != 0; ++i)
            button_color_draw(colors[i], &app);

        // TODO: draw corners

        SDL_RenderPresent(app.renderer);
    }
    return 0;
}
