#pragma once

#include "raylib.h"
#include "IDrawable.h"
#include "IUpdatable.h"
#include "BulletPool.h"

/*
 * Astronaut.h
 * -----------
 * SRP: Owns all ground-phase player behaviour — horizontal movement,
 *      jump physics, sprite animation, and mouse-aimed shooting.
 *      Does not track score or handle collisions.
 *
 * ISP: Implements IDrawable + IUpdatable only. Not ICollidable —
 *      the game loop passes a raw Rectangle to CollisionSystem for
 *      the player hitbox, avoiding forcing Astronaut to implement
 *      takeDamage semantics it doesn't own.
 *
 * DIP: Receives BulletPool& so it can fire without knowing how the
 *      pool is implemented.
 */

class Astronaut : public IDrawable, public IUpdatable {
public:
    static constexpr float GROUND_Y    = 535.0f;
    static constexpr float GRAVITY     = 1500.0f;
    static constexpr float JUMP_SPEED  = 700.0f;
    static constexpr float WALK_SPEED  = 200.0f;
    static constexpr float FRAME_TIME  = 0.075f;
    static constexpr int   FRAME_COUNT = 8;

    Astronaut(Texture2D texture, BulletPool& bulletPool, int screenW);

    void update(float dt)  override;
    void draw()      const override;

    void reset();

    Rectangle getBounds() const { return rec_; }

private:
    Texture2D  texture_;
    BulletPool& bulletPool_;
    int        screenW_;

    Rectangle  rec_{ 300, GROUND_Y, 165, 155 };
    Rectangle  srcRect_{};
    float      velocityY_{ 0 };
    bool       onAir_{ false };
    float      frameTimer_{ 0 };
    float      frameWidth_{ 0 };

    void handleMovement(float dt);
    void handleJump(float dt);
    void handleShooting(float dt);
    void animateSprite(float dt);
};
