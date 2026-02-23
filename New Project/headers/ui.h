#ifndef UI_H
#define UI_H

/*
 * ui.h - Reusable HUD / menu widgets (Single Responsibility)
 *
 * SOLID Principles Applied:
 *  - Single Responsibility: UI rendering and interaction is isolated
 *    here. No game-logic side-effects occur inside these functions.
 *  - Open/Closed: New widgets (sliders, labels) can be added without
 *    touching existing functions.
 */

#include "raylib.h"
#include <stdbool.h>

/*
 * Draw a clickable button and return true on the frame the player
 * releases the left mouse button while hovering over it.
 */
bool ui_button(Rectangle bounds, const char *text);

/*
 * Draw a health bar.
 *  x, y        - top-left corner
 *  w, h        - dimensions
 *  health      - current value
 *  max_health  - maximum value (determines full bar width)
 */
void ui_health_bar(int x, int y, int w, int h, int health, int max_health);

/*
 * Draw the score in the top-right corner.
 */
void ui_score(int screen_width, int score, int font_size, Color color);

#endif /* UI_H */
