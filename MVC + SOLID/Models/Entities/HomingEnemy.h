#pragma once
#include "ICollidable.h"
#include "raylib.h"

// MODEL entity — SRP: owns movement AI and health only.
// Marked final — LSP: no unintended subclass can break takeDamage contract.
class HomingEnemy final : public ICollidable {
public:
    HomingEnemy();

    void spawn(float x, float y, float speed);
    void update(float dt, Rectangle target);
    void despawn() { alive_ = false; }

    // ICollidable
    Rectangle getBounds()        const override { return rec_; }
    bool      isAlive()          const override { return alive_; }
    float     getHealth()        const override { return health_; }
    void      takeDamage(float a)      override;
    void      kill()                   override;

    float getVelocityX() const { return velocityX_; }

private:
    Rectangle rec_{ 0, 0, 0, 0 };
    float     health_{ 150.0f };
    float     maxHealth_{ 150.0f };
    bool      alive_{ false };
    float     velocityX_{ 0 };
    float     velocityY_{ 0 };
    bool      tracking_{ false };
    float     detectionRange_{ 600.0f };
    float     chaseSpeedY_{ 140.0f };
    float     width_{ 0 };
    float     height_{ 0 };

    void updateTracking(Rectangle target);
    void clampPosition();
};
