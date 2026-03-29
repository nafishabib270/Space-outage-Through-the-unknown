#include "BulletPool.h"

BulletPool::BulletPool(int poolSize, Texture2D bulletTexture)
    : pool_(poolSize)
{
    for (auto& b : pool_) b.setTexture(bulletTexture);
}

void BulletPool::fire(Rectangle origin, Vector2 direction, float speed) {
    pool_[nextSlot_].activate(origin, direction, speed);
    nextSlot_ = (nextSlot_ + 1) % static_cast<int>(pool_.size());
}

void BulletPool::update(float dt, int screenWidth) {
    for (auto& b : pool_) b.update(dt, screenWidth);
}

void BulletPool::draw() const {
    for (const auto& b : pool_) b.draw();
}
