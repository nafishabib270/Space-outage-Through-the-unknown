#pragma once
#include "raylib.h"
#include "SessionStats.h"

// VIEW — SRP: HUD rendering only. Reads SessionStats; never mutates it.
class HUD {
public:
    HUD(int screenW, int screenH);
    void render(const SessionStats& stats, bool onSpaceship) const;
private:
    int screenW_, screenH_;
    void drawHealthBar(int hp, int max) const;
    void drawScore(int score) const;
};
