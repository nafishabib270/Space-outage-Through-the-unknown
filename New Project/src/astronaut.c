#include "astronaut.h"

void astronaut_init(AstronautState *a, Texture2D tex) {
    a->frame_width  = tex.width / 8.0f;
    a->tex_width    = tex.width;
    a->frame_time   = 0.075f;
    a->frame_timer  = 0;
    a->velocity_y   = 0;
    a->on_air       = 0;

    a->rec = (Rectangle){ 300, GROUND_Y, 165, 155 };
    a->src = (Rectangle){ 0, 0, a->frame_width, (float)tex.height };
}

void astronaut_update(AstronautState *a, float gravity, float jump_speed,
                      float dt, int screen_w)
{
    /* Horizontal movement. */
    if (IsKeyDown(KEY_A)) a->rec.x -= 200.0f * dt;
    if (IsKeyDown(KEY_D)) a->rec.x += 200.0f * dt;

    /* Screen-edge clamping. */
    if (a->rec.x < 0)                          a->rec.x = 0;
    if (a->rec.x + a->rec.width > screen_w)    a->rec.x = screen_w - a->rec.width;

    /* Jump. */
    if (IsKeyPressed(KEY_W) && !a->on_air) {
        a->on_air    = 1;
        a->velocity_y = -jump_speed;
    }

    /* Gravity. */
    a->velocity_y += gravity * dt;
    a->rec.y      += a->velocity_y * dt;

    /* Ground collision. */
    if (a->rec.y >= GROUND_Y) {
        a->rec.y      = GROUND_Y;
        a->velocity_y = 0;
        a->on_air     = 0;
    }

    /* Sprite animation (only while on ground). */
    if (!a->on_air) {
        a->frame_timer += dt;
        if (a->frame_timer >= a->frame_time) {
            a->src.x   += a->frame_width;
            a->frame_timer = 0;
        }
        if (a->src.x >= a->tex_width) a->src.x = 0;
    }
}

void astronaut_draw(const AstronautState *a, Texture2D tex) {
    DrawTexturePro(tex, a->src, a->rec, (Vector2){0, 0}, 0.0f, WHITE);
}
