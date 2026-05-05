#include "AlienSpawner.h"

AlienSpawner::AlienSpawner(int poolSize, float speed)
    : pool_(poolSize), speed_(speed) { randomiseNextSpawn(); }

void AlienSpawner::randomiseNextSpawn() {
    nextSpawn_ = static_cast<float>(GetRandomValue(5, 10));
}

void AlienSpawner::update(float dt, int screenW) {
    for (auto& a : pool_) a.update(dt);

    timer_ += dt;   // incremented ONCE — fixes original double-increment bug
    if (timer_ < nextSpawn_) return;

    for (auto& a : pool_) {
        if (!a.isAlive()) {
            a.spawn(static_cast<float>(screenW) + 10.0f, speed_);
            break;
        }
    }
    timer_ = 0;
    randomiseNextSpawn();
}

void AlienSpawner::reset() {
    for (auto& a : pool_) a.despawn();
    timer_ = 0;
    randomiseNextSpawn();
}

std::vector<ICollidable*> AlienSpawner::getCollidables() {
    std::vector<ICollidable*> out;
    for (auto& a : pool_) if (a.isAlive()) out.push_back(&a);
    return out;
}
