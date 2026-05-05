#include "EnemySpawner.h"
#include <cstdlib>

EnemySpawner::EnemySpawner(int poolSize, float startSpeed, float startInterval)
    : pool_(poolSize), timeLimit_(startInterval),
      speed_(startSpeed), startSpeed_(startSpeed), startInterval_(startInterval) {}

void EnemySpawner::update(float dt, int screenW, int screenH, Rectangle target) {
    for (auto& e : pool_) e.update(dt, target);

    timer_ += dt;
    if (timer_ < timeLimit_) return;

    if (!pool_[slot_].isAlive())
        pool_[slot_].spawn(screenW + 10.0f,
            static_cast<float>(rand() % static_cast<int>(screenH * 0.8f)), speed_);

    timer_ = 0;
    slot_  = (slot_ + 1) % static_cast<int>(pool_.size());
    if (slot_ == 0) {
        speed_ += 2.0f;
        if (timeLimit_ > minTimeLimit_) timeLimit_ -= 0.1f;
    }
}

void EnemySpawner::reset(float startSpeed, float startInterval) {
    for (auto& e : pool_) e.despawn();
    timer_     = 0;
    slot_      = 0;
    speed_     = startSpeed;
    timeLimit_ = startInterval;
}

std::vector<ICollidable*> EnemySpawner::getCollidables() {
    std::vector<ICollidable*> out;
    for (auto& e : pool_) if (e.isAlive()) out.push_back(&e);
    return out;
}
