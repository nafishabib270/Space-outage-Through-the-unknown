#include "view/MenuView.h"

MenuView::MenuView(int sw, int sh) : screenW_(sw), screenH_(sh) {}

bool MenuView::drawButton(Rectangle b, const char* text) const {
    Vector2 m = GetMousePosition();
    bool hover = CheckCollisionPointRec(m, b);
    DrawRectangleRec(b, hover ? DARKGRAY : GRAY);
    DrawRectangleLines((int)b.x, (int)b.y, (int)b.width, (int)b.height, BLACK);
    DrawText(text, (int)b.x + 20, (int)b.y + 10, 30, BLACK);
    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void MenuView::render(const SessionStats& stats) const {
    ClearBackground(BLACK);
    int tw = MeasureText(" Space Outage ", 60);
    DrawText(" Space Outage ", screenW_ / 2 - tw / 2, 200, 60, DARKBLUE);

    const char* sc = TextFormat("SCORE : %d", stats.getScore());
    DrawText(sc, screenW_ - MeasureText(sc, 60) - 15, 10, 60, RED);

    drawButton(playBtn_, "PLAY");
    drawButton(exitBtn_, "EXIT");
}

GameState MenuView::handleInput() const {
    Vector2 m = GetMousePosition();
    if (CheckCollisionPointRec(m, playBtn_) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        return GameState::Playing;
    if (CheckCollisionPointRec(m, exitBtn_) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        return GameState::Exit;
    return GameState::Menu;
}
