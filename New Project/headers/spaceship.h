#ifndef SPACESHIP_H
#define SPACESHIP_H

/*
 * spaceship.h - Spaceship input and physics (Single Responsibility)
 *
 * SOLID Principles Applied:
 *  - Single Responsibility: All spaceship movement, speed ramping, and
 *    crash physics that were inlined in main.c are collected here.
 *    One reason to change: if spaceship controls need tweaking, only
 *    this module is touched.
 *  - Open/Closed: SpaceshipState is a plain data struct. Behaviours
 *    can be extended (e.g. shield, boost) without modifying existing
 *    update logic.
 */

#include "raylib.h"

typedef struct {
    Rectangle rec;
    float     speed;
    float     initial_speed;
    float     acceleration;
    float     friction;
    float     rotation;
    float     downfall_velocity; /* Used during crash sequence. */
} SpaceshipState;

/* Fill in default values for the spaceship. `tex` is used for sizing. */
void spaceship_init(SpaceshipState *s, int screen_w, int screen_h,
                    int tex_w, int tex_h);

/* Handle WASD input and move the ship. Clamps within screen bounds. */
void spaceship_handle_input(SpaceshipState *s, int screen_w, int screen_h, float dt);

/*
 * Simulate the ship falling after health reaches zero.
 * Returns 1 if the ship has hit the ground (triggers level transition).
 */
int  spaceship_update_crash(SpaceshipState *s, float gravity, int screen_h, float dt);

#endif /* SPACESHIP_H */
