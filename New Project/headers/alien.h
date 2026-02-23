#ifndef ALIEN_H
#define ALIEN_H

/*
 * alien.h - Ground-level animated alien type (Single Responsibility)
 *
 * SOLID Principles Applied:
 *  - Single Responsibility: Alien owns animation state (frame_timer,
 *    cur_frame) in addition to movement. It does NOT do collision.
 *  - Open/Closed: Adding Alien2 or Alien3 means creating a new file,
 *    not editing alien.h/.c.
 *  - Liskov Substitution: alien.base is an EntityBase so the collision
 *    module can treat it the same as an Enemy.
 */

#include "raylib.h"
#include "entity.h"

#define MAX_ALIENS       20
#define ALIEN_FRAME_CNT   8

typedef struct {
    EntityBase base;       /* LSP common contract */
    float      velocity_x;
    float      frame_timer;
    int        cur_frame;
} Alien;

void  alien_init_pool (Alien pool[], int count);
void  alien_spawn     (Alien *a, int screen_w, int screen_h, float speed);
void  alien_update_all(Alien pool[], int count, float dt);
void  alien_draw_all  (const Alien pool[], int count, Texture2D tex);

#endif /* ALIEN_H */
