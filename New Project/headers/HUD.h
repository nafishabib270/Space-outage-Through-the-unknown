#pragma once

#include "raylib.h"
#include "SessionStats.h"

/*
 * HUD.h
 * -----
 * SRP: The one class responsible for all heads-up-display rendering.
 *      Health bar, score text, and any future overlays live here.
 *      In the original code DrawHealthBar() was defined inside main.c
 *      and score was drawn in multiple places inconsistently.
 *
 * DIP: Receives const SessionStats& — depends on the stats abstraction,
 *      not on raw int globals.
 *
 * OCP: Adding a new HUD element (minimap, combo counter) means adding
 *      a new method here — no existing methods are modified.
 *
 * ISP: HUD is not an IDrawable because it needs SessionStats context
 *      that the generic draw() signature cannot carry. Giving it a
 *      tailored render() keeps callers clean.
 */

class HUD {
public:
    HUD(int screenW, int screenH);

    void render(const SessionStats& stats, bool onSpaceship) const;

private:
    int screenW_;
    int screenH_;

    void drawHealthBar(int health, int maxHealth) const;
    void drawScore(int score) const;
};
