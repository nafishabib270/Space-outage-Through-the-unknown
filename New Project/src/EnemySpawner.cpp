#include "EnemySpawner.h"
#include <cstdlib>

EnemySpawner::EnemySpawner(Texture2D texture, int poolSize,
                            float startSpeed, float startInterval)
    : timeLimit_(startInterval)
    , speed_(startSpeed)
    , startSpeed_(startSpeed)
    , startInterval_(startInterval)
{
    pool_.reserve(poolSize);
    for (int i = 0; i < poolSize; ++i)
        pool_.emplace_back(texture);
}

void EnemySpawner::update(float dt, int screenW, int screenH) {
    timer_ += dt;
    if (timer_ < timeLimit_) return;

    // Spawn into the current slot if free.
    if (!pool_[slot_].isAlive()) {
        float y = static_cast<float>(rand() % static_cast<int>(screenH * 0.8f));
        pool_[slot_].spawn(static_cast<float>(screenW) + 10.0f, y, speed_);
        pool_[slot_].setTarget(target_);
    }

    timer_ = 0;
    slot_  = (slot_ + 1) % static_cast<int>(pool_.size());

    // Difficulty ramp when the pool cycles fully.
    if (slot_ == 0) {
        speed_ += 2.0f;
        if (timeLimit_ > minTimeLimit_) timeLimit_ -= 0.1f;
    }
}

void EnemySpawner::updateAll(float dt) {
    for (auto& e : pool_) {
        e.setTarget(target_);
        e.update(dt);
    }
}

void EnemySpawner::drawAll() const {
    for (const auto& e : pool_) e.draw();
}

void EnemySpawner::reset() {
    for (auto& e : pool_) e.despawn();
    timer_     = 0;
    slot_      = 0;
    speed_     = startSpeed_;
    timeLimit_ = startInterval_;
}

std::vector<ICollidable*> EnemySpawner::getCollidables() {
    std::vector<ICollidable*> result;
    result.reserve(pool_.size());
    for (auto& e : pool_)
        if (e.isAlive()) result.push_back(&e);
    return result;
}
