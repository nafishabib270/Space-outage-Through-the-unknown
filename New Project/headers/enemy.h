#ifndef ENEMY_H
#define ENEMY_H

/*
 * enemy.h - Homing enemy type (Single Responsibility + Open/Closed)
 *
 * SOLID Principles Applied:
 *  - Single Responsibility: Enemy only knows about its own movement
 *    (homing trajectory) and drawing. Collision is handled by the
 *    dedicated collision module, not inside update_enemy().
 *  - Open/Closed: The enemy is closed for modification but open for
 *    extension. To add a new enemy variant, create a new struct that
 *    embeds EntityBase and write its own update function — no existing
 *    enemy code needs to change.
 *  - Liskov Substitution: Enemy.base (EntityBase) is the common
 *    "currency" used by collision.h, so Enemy can substitute anywhere
 *    an EntityBase* is expected.
 */

#include "raylib.h"
#include "entity.h"

#define MAX_ENEMIES 10

typedef struct {
    EntityBase base;       /* LSP: common contract (rec, health, alive) */
    float      velocity_x;
    float      velocity_y;
    int        tar_locked; /* Has the enemy computed its intercept course? */
} Enemy;

void  enemy_init_pool (Enemy pool[], int count, Texture2D tex);
void  enemy_spawn     (Enemy *e, int screen_w, int screen_h, float speed);
void  enemy_update_all(Enemy pool[], int count, Rectangle target, float dt);
void  enemy_draw_all  (const Enemy pool[], int count, Texture2D tex);

#endif /* ENEMY_H */
