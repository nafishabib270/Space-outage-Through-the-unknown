#include "HUD.h"

HUD::HUD(int screenW, int screenH)
    : screenW_(screenW), screenH_(screenH)
{}

void HUD::drawHealthBar(int health, int maxHealth) const {
    constexpr int X = 25, Y = 25, W = 300, H = 50;

    float ratio       = maxHealth > 0 ? static_cast<float>(health) / maxHealth : 0.0f;
    int   filledWidth = static_cast<int>(W * ratio);

    Color barColor = GREEN;
    if      (health < 30) barColor = RED;
    else if (health < 60) barColor = YELLOW;

    DrawRectangle(X, Y, W, H, GRAY);
    DrawRectangle(X, Y, filledWidth, H, barColor);
    DrawRectangleLines(X, Y, W, H, BLACK);
}

void HUD::drawScore(int score) const {
    constexpr int FONT_SIZE = 60;
    const char*   text      = TextFormat("SCORE : %d", score);
    int           textWidth = MeasureText(text, FONT_SIZE);
    DrawText(text, screenW_ - textWidth - 15, 10, FONT_SIZE, WHITE);
}

void HUD::render(const SessionStats& stats, bool onSpaceship) const {
    int health = onSpaceship ? stats.getSpaceshipHealth()
                             : stats.getAstronautHealth();
    drawHealthBar(health, SessionStats::MAX_HEALTH);
    drawScore(stats.getScore());
}
