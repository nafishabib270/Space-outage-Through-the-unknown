#include <stdlib.h>
#include <time.h>
#include "enemy.h"

void enemy_init_pool(Enemy pool[], int count, Texture2D tex) {
    for (int i = 0; i < count; i++) {
        pool[i].base.alive        = 0;
        pool[i].base.health       = 150.0f;
        pool[i].base.rec.width    = tex.width  / 5.0f;
        pool[i].base.rec.height   = tex.height / 5.0f;
        pool[i].base.rec.x        = 0;
        pool[i].base.rec.y        = 0;
        pool[i].velocity_x        = 0;
        pool[i].velocity_y        = 0;
        pool[i].tar_locked        = 0;
    }
}

void enemy_spawn(Enemy *e, int screen_w, int screen_h, float speed) {
    e->base.rec.x   = (float)screen_w + 10;
    e->base.rec.y   = (float)(rand() % (screen_h - (int)e->base.rec.height));
    e->base.health  = 150.0f;
    e->velocity_x   = speed;
    e->velocity_y   = 0;
    e->tar_locked   = 0;
    e->base.alive   = 1;
}

void enemy_update_all(Enemy pool[], int count, Rectangle target, float dt) {
    for (int i = 0; i < count; i++) {
        if (!pool[i].base.alive) continue;

        pool[i].base.rec.x -= pool[i].velocity_x * dt;
        pool[i].base.rec.y -= pool[i].velocity_y * dt;

        /* Compute intercept trajectory once the enemy is within range. */
        if (!pool[i].tar_locked) {
            float dx = pool[i].base.rec.x - (target.x + target.width);
            if (dx <= 1000 + (rand() % 300)) {
                pool[i].tar_locked = 1;
                float time = (pool[i].velocity_x != 0) ? dx / pool[i].velocity_x : 0;
                float dy   = pool[i].base.rec.y - (target.y + target.height / 2.0f);
                pool[i].velocity_y = (time != 0) ? dy / time : 0;
            }
        }

        /* Despawn when off-screen. */
        if (pool[i].base.rec.x + pool[i].base.rec.width < 0 ||
            pool[i].base.health <= 0) {
            pool[i].base.alive = 0;
        }
    }
}

void enemy_draw_all(const Enemy pool[], int count, Texture2D tex) {
    Rectangle src = {0, 0, (float)tex.width, (float)tex.height};
    for (int i = 0; i < count; i++) {
        if (!pool[i].base.alive) continue;
        DrawTexturePro(tex, src, pool[i].base.rec,
                       (Vector2){0, 0}, 0.0f, WHITE);
    }
}
