#pragma once
#include "Bullet.h"
#include <vector>

// MODEL — SRP: owns pool recycling and shoot cooldown only.
// View renders bullets separately via getBullets().
class BulletPool {
public:
    BulletPool(int size, float cooldown);

    void tickCooldown(float dt)  { cooldownTimer_ += dt; }
    bool canFire()         const { return cooldownTimer_ >= cooldown_; }
    void resetCooldown()         { cooldownTimer_ = 0; }

    void fire(Rectangle origin, Vector2 dir, float speed);
    void update(float dt, int screenWidth);

    const std::vector<Bullet>& getBullets() const { return pool_; }
    std::vector<Bullet>&       getBullets()       { return pool_; }

private:
    std::vector<Bullet> pool_;
    int   nextSlot_{ 0 };
    float cooldown_;
    float cooldownTimer_{ 0 };
};
