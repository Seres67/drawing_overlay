#include <SDL3/SDL_mouse.h>
#include <button.h>
#include <stdbool.h>

bool is_mouse_in_button(const Button button, const float x, const float y)
{
    if ((x >= button.rect.x && x <= button.rect.x + button.rect.w) &&
        (y >= button.rect.y && y <= button.rect.y + button.rect.h)) {
        return true;
    }
    return false;
}

bool is_mouse_in_color_button(const ColorButton button, const float x, const float y)
{
    if ((x >= button.rect.x && x <= button.rect.x + button.rect.w) &&
        (y >= button.rect.y && y <= button.rect.y + button.rect.h)) {
        return true;
    }
    return false;
}

void button_draw(const Button button, const App app)
{
    float x;
    float y;
    SDL_GetMouseState(&x, &y);
    if (is_mouse_in_button(button, x, y))
        SDL_SetRenderDrawColor(app.renderer, 28, 28, 28, 255);
    else
        SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(app.renderer, &button.rect);
    SDL_RenderTexture(app.renderer, button.texture, NULL, &button.rect);
}

void button_color_draw(const ColorButton button, App *app)
{
    float x;
    float y;
    SDL_GetMouseState(&x, &y);
    if (is_mouse_in_color_button(button, x, y))
        SDL_SetRenderDrawColor(app->renderer, button.color.r == 255 ? 255 : button.color.r + 28,
                               button.color.g == 255 ? 255 : button.color.g + 28,
                               button.color.b == 255 ? 255 : button.color.b + 28, button.color.a);
    else
        SDL_SetRenderDrawColor(app->renderer, button.color.r, button.color.g, button.color.b, button.color.a);
    SDL_RenderFillRect(app->renderer, &button.rect);
    SDL_RenderTexture(app->renderer, button.texture, NULL, &button.rect);
}
