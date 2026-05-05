#pragma once
#include "HomingEnemy.h"
#include "ICollidable.h"
#include <vector>

// MODEL SYSTEM — SRP: timed slot-based enemy spawning only.
class EnemySpawner {
public:
    EnemySpawner(int poolSize, float startSpeed, float startInterval);

    void update(float dt, int screenW, int screenH, Rectangle target);
    void reset(float startSpeed, float startInterval);
    void setTimeLimit(float t) { timeLimit_ = t; }

    std::vector<ICollidable*> getCollidables();
    const std::vector<HomingEnemy>& getEnemies() const { return pool_; }
    std::vector<HomingEnemy>&       getEnemies()       { return pool_; }

private:
    std::vector<HomingEnemy> pool_;
    float timer_{ 0 };
    float timeLimit_;
    float minTimeLimit_{ 0.5f };
    int   slot_{ 0 };
    float speed_;
    float startSpeed_;
    float startInterval_;
};
