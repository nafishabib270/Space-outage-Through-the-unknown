#ifndef COLLISION_H
#define COLLISION_H

/*
 * collision.h - Pure collision-detection and damage-application (SRP + DIP)
 *
 * SOLID Principles Applied:
 *  - Single Responsibility: This module's only job is to test overlaps
 *    and apply the resulting damage/kill events. It does not spawn,
 *    draw, or move anything.
 *  - Dependency Inversion: Functions accept EntityBase* abstractions,
 *    not concrete Enemy or Alien types. High-level game logic calls
 *    collision_bullets_vs_entities() without knowing or caring about
 *    the concrete entity type at compile time.
 *  - Open/Closed: Adding a new enemy type does NOT require editing this
 *    file — just pass its EntityBase array to the existing functions.
 *
 * Previously, six separate nested-loop collision blocks were copy-pasted
 * across main.c. They are now unified here.
 */

#include "raylib.h"
#include "entity.h"
#include "bullet.h"
#include "score.h"

/*
 * Test every active bullet against every alive entity.
 * On a hit: entity loses `hit_power` health, bullet deactivates, hit
 * effect is drawn, and if the entity dies session_register_kill() is called.
 *
 * entity_recs   - pointer to the first Rectangle in an array of structs
 *                 (use offsetof-based stride if rec is not the first field,
 *                  or pass pre-built arrays of EntityBase).
 * entities      - array of EntityBase (or any struct whose first member is
 *                 EntityBase, leveraging C struct layout guarantees).
 * entity_count  - length of entities[].
 */
void collision_bullets_vs_entities(
    Bullet        bullets[],   int bullet_count,
    EntityBase    entities[],  int entity_count,
    SessionStats *stats,
    Texture2D     hit_tex,
    Sound         hit_sounds[], int *sound_idx, int sound_count
);

/*
 * Test every alive entity against a player rectangle.
 * On a hit: player loses `damage` health, entity is deactivated.
 * Returns total damage dealt this frame (0 unless KEY_L god-mode is active).
 */
int collision_entities_vs_player(
    EntityBase  entities[], int entity_count,
    Rectangle   player_rec,
    int         damage
);

#endif /* COLLISION_H */
