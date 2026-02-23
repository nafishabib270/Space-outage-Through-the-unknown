#include "spaceship.h"

void spaceship_init(SpaceshipState *s, int screen_w, int screen_h,
                    int tex_w, int tex_h)
{
    s->rec.width        = tex_w / 3.0f;
    s->rec.height       = tex_h / 3.0f;
    s->rec.x            = screen_w * 0.2f;
    s->rec.y            = screen_h * 0.5f;
    s->speed            = 200.0f;
    s->initial_speed    = 200.0f;
    s->acceleration     = 5.0f;
    s->friction         = 10.0f;
    s->rotation         = 0.0f;
    s->downfall_velocity = 0.0f;
}

void spaceship_handle_input(SpaceshipState *s, int screen_w, int screen_h, float dt)
{
    int moving = 0;

    if (IsKeyDown(KEY_W)) {
        if (IsKeyPressed(KEY_W)) { s->speed = s->initial_speed; s->rotation = 0; }
        if (s->rec.y > 0) {
            s->rec.y -= s->speed * dt;
            s->speed  += s->acceleration;
        }
        moving = 1;
    }

    if (IsKeyDown(KEY_S)) {
        if (IsKeyPressed(KEY_S)) { s->speed = s->initial_speed; s->rotation = 0; }
        if (s->rec.y < screen_h - s->rec.height) {
            s->rec.y += s->speed * dt;
            s->speed  += s->acceleration;
        }
        moving = 1;
    }

    if (IsKeyDown(KEY_D)) {
        if (IsKeyPressed(KEY_D)) s->speed = s->initial_speed;
        if (s->rec.x < screen_w - s->rec.width) {
            s->rec.x += s->speed * dt;
            s->speed  += s->acceleration;
        }
        moving = 1;
    }

    if (IsKeyDown(KEY_A)) {
        if (IsKeyPressed(KEY_A)) s->speed = s->initial_speed;
        if (s->rec.x > 0) {
            s->rec.x -= s->speed * dt;
            s->speed  += s->acceleration;
        }
        moving = 1;
    }

    /* Apply friction when no key is held. */
    if (!moving && s->speed > 0)
        s->speed -= s->friction * dt;
}

int spaceship_update_crash(SpaceshipState *s, float gravity, int screen_h, float dt)
{
    s->downfall_velocity += gravity * dt;
    s->rec.y             += s->downfall_velocity * dt;
    return (s->rec.y > screen_h - 300);
}
