#pragma once

#include "GameEntity.h"
#include "raylib.h"

/*
 * GroundAlien.h
 * -------------
 * Concrete entity: an animated ground-level alien that walks toward
 * the player at a fixed height.
 *
 * SRP: Responsible only for sprite animation and horizontal movement.
 *      CollisionSystem handles damage; EnemySpawner/AlienSpawner
 *      handles timing.
 *
 * OCP: Added without modifying HomingEnemy, GameEntity, or any system.
 *
 * LSP: GroundAlien IS-A GameEntity. CollisionSystem treats it exactly
 *      like HomingEnemy via ICollidable — no branching on type.
 */

class GroundAlien final : public GameEntity {
public:
    static constexpr int   FRAME_COUNT = 8;
    static constexpr float FRAME_TIME  = 0.1f;
    static constexpr float GROUND_Y    = 535.0f;

    explicit GroundAlien(Texture2D texture);

    void spawn(float x, float y, float speed) override;
    void update(float dt)                      override;
    void draw()                          const override;

private:
    Texture2D texture_;
    float     frameTimer_{ 0 };
    int       currentFrame_{ 0 };
    int       frameWidth_{ 0 };
};
