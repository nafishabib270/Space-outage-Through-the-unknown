#include "spawner.h"
#include "raylib.h"

/* ---- Enemy spawner ---------------------------------------------------- */

void enemy_spawner_init(EnemySpawner *s, float start_speed) {
    s->timer           = 0;
    s->time_limit      = 1.0f;
    s->min_time_limit  = 0.5f;
    s->slot            = 0;
    s->speed           = start_speed;
    s->speed_increment = 2.0f;
}

void enemy_spawner_update(EnemySpawner *s, Enemy pool[], int pool_size,
                          int screen_w, int screen_h, float dt)
{
    s->timer += dt;
    if (s->timer < s->time_limit) return;

    /* Spawn into the current slot if it is free. */
    if (!pool[s->slot].base.alive)
        enemy_spawn(&pool[s->slot], screen_w, (int)(screen_h * 0.8f), s->speed);

    s->timer = 0;
    s->slot++;

    /* When the whole pool has cycled once, increase difficulty. */
    if (s->slot >= pool_size) {
        s->slot   = 0;
        s->speed += s->speed_increment;
        if (s->time_limit > s->min_time_limit)
            s->time_limit -= 0.1f;
    }
}

/* ---- Alien spawner ---------------------------------------------------- */

void alien_spawner_init(AlienSpawner *s, float speed) {
    s->timer      = 0;
    s->next_spawn = (float)GetRandomValue(2, 5);
    s->speed      = speed;
}

void alien_spawner_update(AlienSpawner *s, Alien pool[], int pool_size,
                          int screen_w, int screen_h, float dt)
{
    s->timer += dt;
    if (s->timer < s->next_spawn) return;

    /* Find the first inactive slot and spawn there. */
    for (int i = 0; i < pool_size; i++) {
        if (!pool[i].base.alive) {
            alien_spawn(&pool[i], screen_w, screen_h, s->speed);
            break;
        }
    }

    s->timer      = 0;
    s->next_spawn = (float)GetRandomValue(5, 10);
}
