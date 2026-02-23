#ifndef BOOSTER_H
#define BOOSTER_H

/*
 * booster.h - Visual particle effects for ship thrusters (Single Responsibility)
 *
 * SOLID Principles Applied:
 *  - Single Responsibility: This module only manages thruster particle
 *    emission, lifetime, and drawing. It does not touch game state.
 *  - Interface Segregation: Three focused functions rather than one
 *    mega-function. Callers include only what they actually need.
 */

#include "raylib.h"

#define MAX_PARTICLES 100

typedef struct {
    Vector2 pos;
    float   radius;
    float   life;      /* 1.0 = fresh, 0.0 = dead */
    Color   color;
    int     active;
} Particle;

/* Zero-out the pool. */
void booster_init   (Particle pool[MAX_PARTICLES]);

/* Emit & age rear-thruster particles anchored to `ship`. */
void booster_update (Particle pool[MAX_PARTICLES], Rectangle ship);

/*
 * Emit & age side-thruster particles.
 * direction:  1 = going up (downward exhaust), -1 = going down (upward exhaust).
 */
void booster_update_side(Particle pool[MAX_PARTICLES], Rectangle ship, int direction);

/* Age particles that exist without emitting new ones (idle coast). */
void booster_coast  (Particle pool[MAX_PARTICLES]);

/* Draw all active particles with alpha fade. */
void booster_draw   (const Particle pool[MAX_PARTICLES]);

#endif /* BOOSTER_H */
