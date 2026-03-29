#pragma once

#include "Bullet.h"
#include "raylib.h"
#include <vector>
#include <functional>

/*
 * BulletPool.h
 * ------------
 * SRP: Manages a fixed-size pool of Bullet objects — allocation,
 *      recycling, updating, and drawing. The Bullet class itself
 *      stays simple and unaware of pool mechanics.
 *
 * OCP: Pool size and bullet behaviour can be extended without
 *      modifying BulletPool's interface.
 *
 * DIP: CollisionSystem receives const std::vector<Bullet>& via
 *      getBullets() — it depends on a standard container, not
 *      on BulletPool's internal implementation.
 */

class BulletPool {
public:
    explicit BulletPool(int poolSize, Texture2D bulletTexture);

    void fire(Rectangle origin, Vector2 direction, float speed);
    void update(float dt, int screenWidth);
    void draw()   const;

    // Expose bullets for collision queries (read-only view).
    const std::vector<Bullet>& getBullets() const { return pool_; }
    std::vector<Bullet>&       getBullets()       { return pool_; }

private:
    std::vector<Bullet> pool_;
    int                 nextSlot_{ 0 };
    float               cooldown_{ 0.1f };
    float               cooldownTimer_{ 0.0f };

public:
    void tickCooldown(float dt) { cooldownTimer_ += dt; }
    bool canFire()        const { return cooldownTimer_ >= cooldown_; }
    void resetCooldown()        { cooldownTimer_ = 0; }
};
