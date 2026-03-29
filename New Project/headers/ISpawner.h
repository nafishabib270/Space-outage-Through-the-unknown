#pragma once

#include "ICollidable.h"
#include <vector>

/*
 * ISpawner.h
 * ----------
 * DIP: GameWorld holds std::vector<ISpawner*> and calls update(dt)
 *      on each. It never imports EnemySpawner.h or AlienSpawner.h.
 *
 * OCP: A BossSpawner or WaveSpawner can be introduced by implementing
 *      this interface — GameWorld does not change.
 *
 * ISP: Spawners expose only what consumers need: update timing, and
 *      access to the entities they manage for collision queries.
 */

class ISpawner {
public:
    virtual void update(float dt, int screenW, int screenH) = 0;

    // Returns pointers to managed entities so CollisionSystem can
    // test them without knowing the concrete spawner type.
    virtual std::vector<ICollidable*> getCollidables() = 0;

    virtual void reset() = 0;
    virtual void drawAll() const = 0;
    virtual void updateAll(float dt) = 0;

    virtual ~ISpawner() = default;
};
