#pragma once
#include "ICollidable.h"
#include "raylib.h"

// MODEL entity — SRP: owns position, velocity, active state only.
// No draw logic (that is View). No pool logic (that is BulletPool).
class Bullet : public ICollidable {
public:
    void activate(Rectangle origin, Vector2 direction, float speed);
    void deactivate();
    void update(float dt, int screenWidth);

    // ICollidable
    Rectangle getBounds()        const override { return rec_; }
    bool      isAlive()          const override { return active_; }
    float     getHealth()        const override { return 1.0f; }
    void      takeDamage(float)        override { deactivate(); }
    void      kill()                   override { deactivate(); }

    bool isActive() const { return active_; }

private:
    Rectangle rec_{ 0, 0, 20, 10 };
    Vector2   velocity_{ 0, 0 };
    bool      active_{ false };
};
