#include "MenuScreen.h"

MenuScreen::MenuScreen(int screenW, int screenH)
    : screenW_(screenW), screenH_(screenH)
{}

bool MenuScreen::drawButton(Rectangle bounds, const char* text) const {
    Vector2 mouse = GetMousePosition();
    bool    hover = CheckCollisionPointRec(mouse, bounds);

    DrawRectangleRec(bounds, hover ? DARKGRAY : GRAY);
    DrawRectangleLines(
        static_cast<int>(bounds.x), static_cast<int>(bounds.y),
        static_cast<int>(bounds.width), static_cast<int>(bounds.height),
        BLACK);
    DrawText(text,
             static_cast<int>(bounds.x) + 20,
             static_cast<int>(bounds.y) + 10,
             30, BLACK);

    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

GameState MenuScreen::update(const SessionStats& stats) {
    // Draw is called from Game loop; update only reads input.
    if (drawButton(playBtn_, "PLAY")) return GameState::Playing;
    if (drawButton(exitBtn_, "EXIT")) return GameState::Exit;
    return GameState::Menu;
}

void MenuScreen::draw(const SessionStats& stats) const {
    ClearBackground(BLACK);

    DrawText(" Space Outage ",
             screenW_ / 2 - MeasureText(" Space Outage ", 60) / 2,
             200, 60, DARKBLUE);

    // Show last-session score in top-right corner.
    const char* scoreText  = TextFormat("SCORE : %d", stats.getScore());
    int         scoreWidth = MeasureText(scoreText, 60);
    DrawText(scoreText, screenW_ - scoreWidth - 15, 10, 60, RED);

    // Buttons are drawn here so hover state is visible during draw.
    // (update() is called *after* draw in the game loop, which means
    //  the button highlight and the click test happen in the same frame.)
    Vector2 mouse = GetMousePosition();

    auto buttonDraw = [&](Rectangle b, const char* label) {
        bool hover = CheckCollisionPointRec(mouse, b);
        DrawRectangleRec(b, hover ? DARKGRAY : GRAY);
        DrawRectangleLines(
            static_cast<int>(b.x), static_cast<int>(b.y),
            static_cast<int>(b.width), static_cast<int>(b.height), BLACK);
        DrawText(label,
                 static_cast<int>(b.x) + 20,
                 static_cast<int>(b.y) + 10,
                 30, BLACK);
    };

    buttonDraw(playBtn_, "PLAY");
    buttonDraw(exitBtn_, "EXIT");
}
