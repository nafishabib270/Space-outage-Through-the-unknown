#ifndef SPAWNER_H
#define SPAWNER_H

/*
 * spawner.h - Timed enemy/alien spawning (Open/Closed + SRP)
 *
 * SOLID Principles Applied:
 *  - Single Responsibility: All spawn-timing logic that was previously
 *    scattered across main.c (enemy_timer, enemy_now, alien_timer,
 *    next_alien_spawn) now lives in one place.
 *  - Open/Closed: EnemySpawner is closed for modification but open for
 *    extension: subclass it (new struct) to change spawn patterns
 *    without editing the base spawner code.
 *  - Dependency Inversion: Spawner depends on Enemy/Alien abstractions
 *    via function pointers (or plain calls), not on game-loop internals.
 */

#include "enemy.h"
#include "alien.h"

/* ---- Enemy spawner ---------------------------------------------------- */

typedef struct {
    float timer;
    float time_limit;      /* Current interval between spawns (seconds). */
    float min_time_limit;  /* Fastest the interval can shrink to. */
    int   slot;            /* Round-robin index into enemy pool. */
    float speed;           /* Current enemy movement speed. */
    float speed_increment; /* How much speed grows per full pool cycle. */
} EnemySpawner;

void  enemy_spawner_init  (EnemySpawner *s, float start_speed);
void  enemy_spawner_update(EnemySpawner *s, Enemy pool[], int pool_size,
                           int screen_w, int screen_h, float dt);

/* ---- Alien spawner ---------------------------------------------------- */

typedef struct {
    float timer;
    float next_spawn; /* Randomised delay until next alien appears. */
    float speed;
} AlienSpawner;

void  alien_spawner_init  (AlienSpawner *s, float speed);
void  alien_spawner_update(AlienSpawner *s, Alien pool[], int pool_size,
                           int screen_w, int screen_h, float dt);

#endif /* SPAWNER_H */
