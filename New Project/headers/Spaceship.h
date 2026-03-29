#pragma once

#include "raylib.h"
#include "IDrawable.h"
#include "IUpdatable.h"
#include "ThrusterEffect.h"
#include "BulletPool.h"

/*
 * Spaceship.h
 * -----------
 * SRP: Owns WASD movement, speed ramping, crash physics, and thruster
 *      particle delegation. Does not track score or handle collisions.
 *
 * ISP: Implements IDrawable and IUpdatable. It does NOT implement
 *      ICollidable — the game loop tests a raw Rectangle against
 *      enemy positions, keeping the spaceship hitbox simple.
 *
 * OCP: Crash physics and thruster behaviour are separate private
 *      helpers — new movement modes (boost, shield) are additions.
 *
 * DIP: Receives BulletPool& through the constructor. The spaceship
 *      does not know how bullets are stored — only how to fire one.
 */

class Spaceship : public IDrawable, public IUpdatable {
public:
    static constexpr float INITIAL_SPEED    = 200.0f;
    static constexpr float ACCELERATION     = 5.0f;
    static constexpr float FRICTION         = 10.0f;
    static constexpr float GRAVITY          = 1500.0f;
    static constexpr float SHOOT_COOLDOWN   = 0.1f;

    Spaceship(Texture2D texture, BulletPool& bulletPool,
              int screenW, int screenH);

    void update(float dt)  override;
    void draw()      const override;

    void reset(int screenW, int screenH);

    Rectangle getBounds()       const { return rec_; }
    float     getRotation()     const { return rotation_; }
    bool      isCrashing()      const { return crashing_; }
    bool      hasCrashLanded()  const { return crashLanded_; }

    void startCrash() { crashing_ = true; }

private:
    Texture2D      texture_;
    BulletPool&    bulletPool_;
    Rectangle      rec_{ 0, 0, 0, 0 };
    float          speed_{ INITIAL_SPEED };
    float          rotation_{ 0 };
    float          downfallVelocity_{ 0 };
    float          bulletTimer_{ 0 };
    bool           crashing_{ false };
    bool           crashLanded_{ false };
    int            screenW_;
    int            screenH_;
    ThrusterEffect rearThrusters_;
    ThrusterEffect sideThrusters_;

    void handleInput(float dt);
    void updateCrash(float dt);
};
