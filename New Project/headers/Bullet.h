#pragma once

#include "raylib.h"
#include "IDrawable.h"
#include "ICollidable.h"

/*
 * Bullet.h
 * --------
 * SRP: A bullet owns its position, velocity, and active state only.
 *      Firing logic lives in the player controllers (Spaceship,
 *      Astronaut). Collision resolution lives in CollisionSystem.
 *
 * ISP: Bullet implements ICollidable (for hit tests) and IDrawable
 *      (for rendering) but NOT IUpdatable via the interface — its
 *      pool is updated by BulletPool which owns the update loop,
 *      keeping Bullet itself simple.
 *
 * Note: Bullet does NOT extend GameEntity because it has no health,
 *       no spawn timer, and no AI. Forcing it to inherit those would
 *       violate ISP (it would inherit methods it doesn't use).
 */

class Bullet : public ICollidable, public IDrawable {
public:
    Bullet();

    void activate(Rectangle origin, Vector2 direction, float speed);
    void deactivate();
    void update(float dt, int screenWidth);

    // ---- ICollidable ----
    Rectangle getBounds()         const override;
    bool      isAlive()           const override;
    float     getHealth()         const override { return 1.0f; }
    void      takeDamage(float)         override { deactivate(); }
    void      kill()                    override { deactivate(); }

    // ---- IDrawable ----
    void draw() const override;

    bool isActive() const { return active_; }
    void setTexture(Texture2D tex) { texture_ = tex; }

private:
    Rectangle rec_{ 0, 0, 20, 10 };
    Vector2   velocity_{ 0, 0 };
    bool      active_{ false };
    Texture2D texture_{};
};
