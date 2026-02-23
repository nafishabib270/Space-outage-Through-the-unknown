#include "ui.h"

bool ui_button(Rectangle bounds, const char *text) {
    Vector2 mouse = GetMousePosition();
    bool    hover = CheckCollisionPointRec(mouse, bounds);

    DrawRectangleRec(bounds, hover ? DARKGRAY : GRAY);
    DrawRectangleLines((int)bounds.x, (int)bounds.y,
                       (int)bounds.width, (int)bounds.height, BLACK);
    DrawText(text, (int)bounds.x + 20, (int)bounds.y + 10, 30, BLACK);

    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void ui_health_bar(int x, int y, int w, int h, int health, int max_health) {
    float ratio       = (max_health > 0) ? (float)health / max_health : 0;
    int   filled      = (int)(w * ratio);

    DrawRectangle(x, y, w, h, GRAY);

    Color bar_color = GREEN;
    if (health < 30) bar_color = RED;
    else if (health < 60) bar_color = YELLOW;

    DrawRectangle(x, y, filled, h, bar_color);
    DrawRectangleLines(x, y, w, h, BLACK);
}

void ui_score(int screen_width, int score, int font_size, Color color) {
    const char *text  = TextFormat("SCORE : %d", score);
    int         width = MeasureText(text, font_size);
    DrawText(text, screen_width - width - 15, 10, font_size, color);
}
