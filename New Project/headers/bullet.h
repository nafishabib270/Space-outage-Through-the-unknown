#ifndef BULLET_H
#define BULLET_H

/*
 * bullet.h - Bullet lifecycle management (Single Responsibility)
 *
 * SOLID Principles Applied:
 *  - Single Responsibility: Bullets own their own position, velocity,
 *    and active state. All bullet mutation happens through this module.
 *  - Open/Closed: New bullet types (e.g. homing, spread) can be added
 *    by extending the Bullet struct and adding specialised spawn
 *    functions without modifying existing update/draw paths.
 *  - Dependency Inversion: draw_bullets() depends on a Texture2D passed
 *    in from outside, not on a global. Callers own their resources.
 */

#include "raylib.h"

#define MAX_BULLETS 50

typedef struct {
    Rectangle rec;
    Vector2   velocity;
    int       active;
} Bullet;

void bullet_init_pool (Bullet pool[], int count);
void bullet_spawn     (Bullet *b, Rectangle origin, Vector2 dir, float speed);
void bullet_update_all(Bullet pool[], int count, float dt, int screen_width);
void bullet_draw_all  (const Bullet pool[], int count, Texture2D tex);

#endif /* BULLET_H */
