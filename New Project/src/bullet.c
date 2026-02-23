#include "bullet.h"

void bullet_init_pool(Bullet pool[], int count) {
    for (int i = 0; i < count; i++) {
        pool[i].active     = 0;
        pool[i].rec        = (Rectangle){0};
        pool[i].velocity   = (Vector2){0, 0};
    }
}

void bullet_spawn(Bullet *b, Rectangle origin, Vector2 dir, float speed) {
    b->rec.x      = origin.x + origin.width  / 2.0f;
    b->rec.y      = origin.y + origin.height / 2.0f;
    b->rec.width  = 20.0f;
    b->rec.height = 10.0f;
    b->velocity.x = dir.x * speed;
    b->velocity.y = dir.y * speed;
    b->active     = 1;
}

void bullet_update_all(Bullet pool[], int count, float dt, int screen_width) {
    for (int i = 0; i < count; i++) {
        if (!pool[i].active) continue;
        pool[i].rec.x += pool[i].velocity.x * dt;
        pool[i].rec.y += pool[i].velocity.y * dt;
        /* Deactivate once off-screen. */
        if (pool[i].rec.x > screen_width || pool[i].rec.x < -50 ||
            pool[i].rec.y < -50 || pool[i].rec.y > 2000) {
            pool[i].active = 0;
        }
    }
}

void bullet_draw_all(const Bullet pool[], int count, Texture2D tex) {
    Rectangle src = {0, 0, (float)tex.width, (float)tex.height};
    for (int i = 0; i < count; i++) {
        if (!pool[i].active) continue;
        DrawTexturePro(tex, src, pool[i].rec, (Vector2){0, 0}, 0.0f, WHITE);
    }
}
