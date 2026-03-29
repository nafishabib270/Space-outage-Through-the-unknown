#include "AlienSpawner.h"

AlienSpawner::AlienSpawner(Texture2D texture, int poolSize, float speed)
    : speed_(speed)
{
    pool_.reserve(poolSize);
    for (int i = 0; i < poolSize; ++i)
        pool_.emplace_back(texture);
    randomiseNextSpawn();
}

void AlienSpawner::randomiseNextSpawn() {
    nextSpawn_ = static_cast<float>(GetRandomValue(5, 10));
}

void AlienSpawner::update(float dt, int screenW, int screenH) {
    timer_ += dt;
    if (timer_ < nextSpawn_) return;

    for (auto& a : pool_) {
        if (!a.isAlive()) {
            a.spawn(static_cast<float>(screenW) + 10.0f,
                    GroundAlien::GROUND_Y, speed_);
            break;
        }
    }

    timer_ = 0;
    randomiseNextSpawn();
}

void AlienSpawner::updateAll(float dt) {
    for (auto& a : pool_) a.update(dt);
}

void AlienSpawner::drawAll() const {
    for (const auto& a : pool_) a.draw();
}

void AlienSpawner::reset() {
    for (auto& a : pool_) a.despawn();
    timer_ = 0;
    randomiseNextSpawn();
}

std::vector<ICollidable*> AlienSpawner::getCollidables() {
    std::vector<ICollidable*> result;
    result.reserve(pool_.size());
    for (auto& a : pool_)
        if (a.isAlive()) result.push_back(&a);
    return result;
}
