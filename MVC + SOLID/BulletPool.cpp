#include "model/entities/BulletPool.h"

BulletPool::BulletPool(int size, float cooldown)
    : pool_(size), cooldown_(cooldown) {}

void BulletPool::fire(Rectangle origin, Vector2 dir, float speed) {
    pool_[nextSlot_].activate(origin, dir, speed);
    nextSlot_ = (nextSlot_ + 1) % static_cast<int>(pool_.size());
}

void BulletPool::update(float dt, int sw) {
    for (auto& b : pool_) b.update(dt, sw);
}
