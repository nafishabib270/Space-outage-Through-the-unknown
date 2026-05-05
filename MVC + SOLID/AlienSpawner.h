#pragma once
#include "model/entities/GroundAlien.h"
#include "core/ICollidable.h"
#include <vector>

// MODEL SYSTEM — SRP: randomised alien spawning only.
// Fixes the original double alien_timer += dt bug.
class AlienSpawner {
public:
    AlienSpawner(int poolSize, float speed);

    void update(float dt, int screenW);   // timer incremented ONCE per frame
    void reset();

    std::vector<ICollidable*> getCollidables();
    const std::vector<GroundAlien>& getAliens() const { return pool_; }
    std::vector<GroundAlien>&       getAliens()       { return pool_; }

private:
    std::vector<GroundAlien> pool_;
    float timer_{ 0 };
    float nextSpawn_{ 0 };
    float speed_;

    void randomiseNextSpawn();
};
