#include "view/HUD.h"

HUD::HUD(int sw, int sh) : screenW_(sw), screenH_(sh) {}

void HUD::drawHealthBar(int hp, int max) const {
    float ratio = max > 0 ? static_cast<float>(hp) / max : 0;
    Color c = hp < 30 ? RED : hp < 60 ? YELLOW : GREEN;
    DrawRectangle(25, 25, 300, 50, GRAY);
    DrawRectangle(25, 25, static_cast<int>(300 * ratio), 50, c);
    DrawRectangleLines(25, 25, 300, 50, BLACK);
}

void HUD::drawScore(int score) const {
    const char* t = TextFormat("SCORE : %d", score);
    int w = MeasureText(t, 60);
    DrawText(t, screenW_ - w - 15, 10, 60, WHITE);
}

void HUD::render(const SessionStats& s, bool onShip) const {
    int hp = onShip ? s.getSpaceshipHealth() : s.getAstronautHealth();
    drawHealthBar(hp, SessionStats::MAX_HEALTH);
    drawScore(s.getScore());
}
