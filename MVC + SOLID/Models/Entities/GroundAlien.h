#pragma once
#include "ICollidable.h"
#include "raylib.h"

// MODEL entity — SRP: animation state + horizontal movement only.
class GroundAlien final : public ICollidable {
public:
    static constexpr float GROUND_Y    = 535.0f;
    static constexpr int   FRAME_COUNT = 8;
    static constexpr float FRAME_TIME  = 0.1f;

    void spawn(float x, float speed);
    void update(float dt);
    void despawn() { alive_ = false; }

    // ICollidable
    Rectangle getBounds()        const override { return rec_; }
    bool      isAlive()          const override { return alive_; }
    float     getHealth()        const override { return health_; }
    void      takeDamage(float a)      override;
    void      kill()                   override;

    int  getCurrentFrame()  const { return currentFrame_; }
    bool isActive()         const { return alive_; }

private:
    Rectangle rec_{ 0, GROUND_Y, 200, 100 };
    float     health_{ 200.0f };
    float     maxHealth_{ 200.0f };
    bool      alive_{ false };
    float     velocityX_{ 0 };
    float     frameTimer_{ 0 };
    int       currentFrame_{ 0 };
};
