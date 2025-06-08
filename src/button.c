#include <button.h>
#include <stdbool.h>

bool is_button_clicked(Button button, float x, float y)
{
    if ((x >= button.rect.x && x <= button.rect.x + button.rect.w) &&
        (y >= button.rect.y && y <= button.rect.y + button.rect.h)) {
        return true;
    }
    return false;
}
