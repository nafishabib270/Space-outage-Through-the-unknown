#ifndef ASTRONAUT_H
#define ASTRONAUT_H

/*
 * astronaut.h - Ground-phase player character (Single Responsibility)
 *
 * SOLID Principles Applied:
 *  - Single Responsibility: All astronaut movement, jumping, gravity,
 *    and sprite animation that was inlined in main.c lives here.
 *  - Open/Closed: Double-jump or dash abilities can be added via new
 *    fields in AstronautState without touching other systems.
 */

#include "raylib.h"

#define GROUND_Y 535.0f

typedef struct {
    Rectangle   rec;
    Rectangle   src;      /* Sprite source rect (for animation). */
    float       velocity_y;
    int         on_air;
    float       frame_timer;
    float       frame_time;
    float       frame_width; /* Width of a single animation frame. */
    int         tex_width;   /* Full texture width (to detect loop). */
} AstronautState;

void  astronaut_init   (AstronautState *a, Texture2D tex);
void  astronaut_update (AstronautState *a, float gravity, float jump_speed,
                        float dt, int screen_w);
void  astronaut_draw   (const AstronautState *a, Texture2D tex);

#endif /* ASTRONAUT_H */
