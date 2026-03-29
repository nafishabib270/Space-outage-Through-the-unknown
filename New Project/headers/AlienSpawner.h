#pragma once

#include "ISpawner.h"
#include "GroundAlien.h"
#include "raylib.h"
#include <vector>

/*
 * AlienSpawner.h
 * --------------
 * SRP: Owns randomised-interval spawning of GroundAlien objects only.
 *
 * OCP: Implements ISpawner. GameWorld treats this identically to
 *      EnemySpawner through the interface — no branching.
 *
 * LSP: getCollidables() returns ICollidable* so CollisionSystem works
 *      on GroundAlien the same way it works on HomingEnemy.
 */

class AlienSpawner final : public ISpawner {
public:
    AlienSpawner(Texture2D texture, int poolSize, float speed);

    void update(float dt, int screenW, int screenH) override;
    void drawAll()                            const override;
    void updateAll(float dt)                        override;
    void reset()                                    override;

    std::vector<ICollidable*> getCollidables()      override;

private:
    std::vector<GroundAlien> pool_;
    float                    timer_{ 0 };
    float                    nextSpawn_{ 0 };
    float                    speed_;

    void randomiseNextSpawn();
};
