#pragma once

#include "ISpawner.h"
#include "HomingEnemy.h"
#include "raylib.h"
#include <vector>

/*
 * EnemySpawner.h
 * --------------
 * SRP: Owns the timed slot-based spawning of HomingEnemy objects and
 *      nothing else. Difficulty escalation (speed ramp, interval
 *      reduction) lives here, not in the game loop.
 *
 * OCP: Implements ISpawner — adding a new spawner type never modifies
 *      this class.
 *
 * LSP: Returns std::vector<ICollidable*> from getCollidables() so
 *      CollisionSystem can treat HomingEnemy as any other ICollidable.
 *
 * DIP: GameWorld holds an ISpawner* to this object, never a concrete
 *      EnemySpawner*.
 */

class EnemySpawner final : public ISpawner {
public:
    EnemySpawner(Texture2D texture, int poolSize,
                 float startSpeed, float startInterval);

    void update(float dt, int screenW, int screenH) override;
    void drawAll()                            const override;
    void updateAll(float dt)                        override;
    void reset()                                    override;

    std::vector<ICollidable*> getCollidables()      override;

    void setTarget(Rectangle target) { target_ = target; }
    void setTimeLimit(float t)       { timeLimit_ = t; }

private:
    std::vector<HomingEnemy> pool_;
    Rectangle                target_{ 0, 0, 0, 0 };
    float                    timer_{ 0 };
    float                    timeLimit_;
    float                    minTimeLimit_{ 0.5f };
    int                      slot_{ 0 };
    float                    speed_;
    float                    startSpeed_;
    float                    startInterval_;
};
