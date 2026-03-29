#pragma once

#include "raylib.h"
#include "IDrawable.h"
#include "IUpdatable.h"
#include <vector>

/*
 * ThrusterEffect.h
 * ----------------
 * SRP: Owns all particle emission, aging, and rendering for thruster
 *      exhaust trails. Does not touch game state or entity data.
 *
 * ISP: Implements IDrawable and IUpdatable only — it has no health,
 *      no collision, and no spawn timer.
 *
 * OCP: Emission patterns (rear, side-up, side-down, coast) are
 *      separate public methods — new patterns are additions, not
 *      modifications to existing ones.
 */

struct ThrusterParticle {
    Vector2 pos{};
    float   radius{ 0 };
    float   life{ 0 };     // 1.0 = fresh, 0.0 = dead
    Color   color{ GRAY };
    bool    active{ false };
};

class ThrusterEffect : public IDrawable, public IUpdatable {
public:
    static constexpr int POOL_SIZE = 100;

    explicit ThrusterEffect(int poolSize = POOL_SIZE);

    // Emission modes — call the one matching the current input.
    void emitRear   (Rectangle shipRect);
    void emitSideUp (Rectangle shipRect);
    void emitSideDown(Rectangle shipRect);
    void coast();           // Age existing particles, emit nothing new.

    // IUpdatable — ages all particles.
    void update(float dt) override;

    // IDrawable — renders all active particles.
    void draw() const override;

    void reset();

private:
    std::vector<ThrusterParticle> pool_;

    void emit(Vector2 origin);
    void ageAll();
};
