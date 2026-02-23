#ifndef ENTITY_H
#define ENTITY_H

/*
 * entity.h - Abstract entity interface (ISP + LSP)
 *
 * SOLID Principles Applied:
 *  - Interface Segregation: Separates "what every entity must do" from
 *    implementation-specific details. No entity is forced to implement
 *    logic it doesn't need.
 *  - Liskov Substitution: All entity types (Enemy, Alien) satisfy this
 *    interface, so they can be used interchangeably in generic systems
 *    (collision detection, health management, etc.)
 *  - Dependency Inversion: High-level systems (collision.h, game_loop.c)
 *    depend on this abstraction, not on Enemy or Alien1 directly.
 */

#include "raylib.h"

/* Every game entity exposes position, size, health, and alive state.
 * This is the shared "contract" that all entity types must honour. */
typedef struct {
    Rectangle rec;   /* World-space bounding box (position + size) */
    float     health;
    int       alive;
} EntityBase;

#endif /* ENTITY_H */
