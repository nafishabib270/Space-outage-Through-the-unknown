#include "alien.h"

#define FRAME_TIME 0.1f

void alien_init_pool(Alien pool[], int count) {
    for (int i = 0; i < count; i++) {
        pool[i].base.alive    = 0;
        pool[i].base.health   = 200.0f;
        pool[i].base.rec      = (Rectangle){0, 0, 200, 100};
        pool[i].velocity_x    = 0;
        pool[i].frame_timer   = 0;
        pool[i].cur_frame     = 0;
    }
}

void alien_spawn(Alien *a, int screen_w, int screen_h, float speed) {
    (void)screen_h; /* Aliens always spawn at the ground row. */
    a->base.rec.x  = (float)screen_w + 10;
    a->base.rec.y  = 535.0f;
    a->base.health = 200.0f;
    a->velocity_x  = speed;
    a->base.alive  = 1;
    a->cur_frame   = 0;
    a->frame_timer = 0;
}

void alien_update_all(Alien pool[], int count, float dt) {
    for (int i = 0; i < count; i++) {
        if (!pool[i].base.alive) continue;

        pool[i].base.rec.x -= pool[i].velocity_x * dt;

        /* Frame animation. */
        pool[i].frame_timer += dt;
        if (pool[i].frame_timer >= FRAME_TIME) {
            pool[i].cur_frame = (pool[i].cur_frame + 1) % ALIEN_FRAME_CNT;
            pool[i].frame_timer = 0;
        }

        /* Despawn when off-screen or dead. */
        if (pool[i].base.rec.x < -pool[i].base.rec.width ||
            pool[i].base.health <= 0) {
            pool[i].base.alive = 0;
        }
    }
}

void alien_draw_all(const Alien pool[], int count, Texture2D tex) {
    int frame_w = tex.width / ALIEN_FRAME_CNT;
    for (int i = 0; i < count; i++) {
        if (!pool[i].base.alive) continue;
        Rectangle src = {
            pool[i].cur_frame * frame_w, 0,
            frame_w, (float)tex.height
        };
        DrawTexturePro(tex, src, pool[i].base.rec,
                       (Vector2){0, 0}, 0.0f, WHITE);
    }
}
