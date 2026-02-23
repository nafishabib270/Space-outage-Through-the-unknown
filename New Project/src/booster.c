#include "booster.h"

void booster_init(Particle pool[MAX_PARTICLES]) {
    for (int i = 0; i < MAX_PARTICLES; i++) pool[i].active = 0;
}

/* --- internal helpers --------------------------------------------------- */

static void emit_particle(Particle pool[MAX_PARTICLES], Vector2 origin) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!pool[i].active) {
            pool[i].pos    = origin;
            pool[i].radius = (float)GetRandomValue(2, 6);
            pool[i].life   = 1.0f;
            pool[i].color  = GRAY;
            pool[i].active = 1;
            return;
        }
    }
}

static void age_particles(Particle pool[MAX_PARTICLES]) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!pool[i].active) continue;
        pool[i].life -= 0.025f;
        if (pool[i].radius > 0) pool[i].radius -= 0.1f;
        if (pool[i].life  <= 0) pool[i].active  = 0;
    }
}

/* ----------------------------------------------------------------------- */

void booster_update(Particle pool[MAX_PARTICLES], Rectangle ship) {
    /* Emit two particles per frame from the rear of the ship. */
    Vector2 origin = {
        ship.x + 3,
        ship.y + ship.height / 2.0f + GetRandomValue(-10, 10)
    };
    for (int i = 0; i < 2; i++) emit_particle(pool, origin);

    /* Age existing particles and drift them left. */
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!pool[i].active) continue;
        pool[i].pos.x -= 3;
        pool[i].pos.y  = ship.y + ship.height / 2.0f + GetRandomValue(-10, 10);
    }
    age_particles(pool);
}

void booster_update_side(Particle pool[MAX_PARTICLES], Rectangle ship, int direction) {
    Vector2 origin = {
        ship.x + ship.width / 5.0f,
        ship.y + ship.height / 2.0f + GetRandomValue(-10, 10)
    };
    for (int i = 0; i < 2; i++) emit_particle(pool, origin);

    float drift_y = (direction == 1) ? 3.0f : -3.0f;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!pool[i].active) continue;
        pool[i].pos.x -= 3;
        pool[i].pos.y += drift_y;
    }
    age_particles(pool);
}

void booster_coast(Particle pool[MAX_PARTICLES]) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!pool[i].active) continue;
        pool[i].pos.x -= 3;
    }
    age_particles(pool);
}

void booster_draw(const Particle pool[MAX_PARTICLES]) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!pool[i].active || pool[i].radius <= 0) continue;
        Color c  = pool[i].color;
        c.a      = (unsigned char)(pool[i].life * 255);
        DrawCircleV(pool[i].pos, pool[i].radius, c);
    }
}
