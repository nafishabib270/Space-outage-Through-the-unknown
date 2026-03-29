#pragma once

#include "GameEntity.h"
#include "raylib.h"

/*
 * HomingEnemy.h
 * -------------
 * Concrete entity: a flying enemy that computes an intercept trajectory
 * when the player enters its detection range.
 *
 * SRP: Only responsible for homing movement and its own rendering.
 *      Collision damage is applied by CollisionSystem via ICollidable.
 *      Spawning timing is managed by EnemySpawner, not this class.
 *
 * OCP: If a new enemy variant is needed (e.g. ShieldedEnemy), a new
 *      class extends GameEntity — HomingEnemy is never modified.
 *
 * LSP: HomingEnemy IS-A GameEntity IS-A ICollidable. The CollisionSystem
 *      handles it correctly through the base interface with no special
 *      casing.
 */

class HomingEnemy final : public GameEntity {
public:
    explicit HomingEnemy(Texture2D texture);

    void spawn(float x, float y, float speed) override;
    void update(float dt)                      override;
    void draw()                          const override;

    void setTarget(Rectangle target);

private:
    Texture2D texture_;
    Rectangle target_{ 0, 0, 0, 0 };
    bool      targetLocked_{ false };

    void computeIntercept();
};
