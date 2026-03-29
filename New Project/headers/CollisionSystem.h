#pragma once

#include "ICollidable.h"
#include "Bullet.h"
#include "SessionStats.h"
#include "raylib.h"
#include <vector>

/*
 * CollisionSystem.h
 * -----------------
 * SRP: The only responsibility of this class is collision detection
 *      and its immediate consequences (damage, death, score). It does
 *      not spawn, render, or move anything.
 *
 * DIP: Depends on ICollidable (abstraction), not on HomingEnemy or
 *      GroundAlien (concretions). The six copy-pasted collision loops
 *      in the original main.c are replaced by two generic methods.
 *
 * OCP: Adding a new entity type that implements ICollidable requires
 *      zero changes here — just pass it to testBulletsVsEntities().
 *
 * LSP: HomingEnemy and GroundAlien are interchangeable as ICollidable
 *      — the system calls getBounds(), takeDamage(), kill() on both
 *      through the interface with identical behaviour.
 */

class CollisionSystem {
public:
    CollisionSystem(SessionStats& stats, Texture2D hitEffect);

    // Test every active bullet against every alive entity.
    // On hit: entity takes damage, bullet deactivates, hit effect drawn,
    //         sound played, kill registered with SessionStats if entity dies.
    void testBulletsVsEntities(
        std::vector<Bullet>&    bullets,
        std::vector<ICollidable*> entities,
        std::vector<Sound>&     hitSounds,
        int&                    soundIndex);

    // Test every alive entity against a player rectangle.
    // Returns total damage dealt this frame (0 in god-mode).
    int testEntitiesVsPlayer(
        std::vector<ICollidable*> entities,
        Rectangle                 playerRect,
        int                       damagePerHit);

private:
    SessionStats& stats_;
    Texture2D     hitEffect_;

    static Rectangle shrink(Rectangle r, float margin);
};
