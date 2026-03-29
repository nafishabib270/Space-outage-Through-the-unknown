#pragma once

#include "raylib.h"
#include "ResourceManager.h"
#include "SessionStats.h"
#include "BulletPool.h"
#include "Spaceship.h"
#include "Astronaut.h"
#include "EnemySpawner.h"
#include "AlienSpawner.h"
#include "CollisionSystem.h"
#include "HUD.h"
#include "ISpawner.h"
#include <vector>
#include <memory>

/*
 * GameWorld.h
 * -----------
 * SRP: Owns and wires together all gameplay systems. Its single
 *      responsibility is coordinating the update/draw order for one
 *      game session. It does not contain any physics math, collision
 *      math, or rendering code itself.
 *
 * DIP: Holds spawners as std::vector<ISpawner*> — depends on the
 *      abstract interface, not on EnemySpawner or AlienSpawner directly.
 *      CollisionSystem receives ICollidable* lists from those spawners.
 *
 * OCP: Adding a new phase (e.g. a boss level) means adding a new
 *      updateBossPhase() / drawBossPhase() private method and a new
 *      ISpawner implementation — existing phase code is untouched.
 */

class GameWorld {
public:
    static constexpr int SCREEN_W      = 1800;
    static constexpr int SCREEN_H      = 900;
    static constexpr int SCROLL_SPEED  = 200;
    static constexpr int ENEMY_DAMAGE  = 20;
    static constexpr int ALIEN_DAMAGE  = 20;

    explicit GameWorld(ResourceManager& resources);

    void reset();
    void update(float dt);
    void draw();

    bool isGameOver() const;

    // Read-only access for the menu to display the last score.
    const SessionStats& getStats() const { return stats_; }

private:
    ResourceManager&                resources_;
    SessionStats                    stats_;
    BulletPool                      bulletPool_;
    Spaceship                       spaceship_;
    Astronaut                       astronaut_;
    std::unique_ptr<EnemySpawner>   enemySpawner_;
    std::unique_ptr<AlienSpawner>   alienSpawner_;
    CollisionSystem                 collisionSystem_;
    HUD                             hud_;
    int                             hitSoundIdx_{ 0 };

    // Scrolling background state.
    Rectangle bgSrc_{};
    Rectangle bgPos1_{};
    Rectangle bgPos2_{};

    // Phase flag: true = flying, false = on foot.
    bool onSpaceship_{ true };
    bool phaseTransitioned_{ false };

    void updateSpaceshipPhase(float dt);
    void updateAstronautPhase(float dt);
    void drawSpaceshipPhase();
    void drawAstronautPhase();
    void scrollBackground(float dt);

    // Helpers to build ICollidable* lists for the collision system.
    std::vector<ICollidable*> getEnemyCollidables();
    std::vector<ICollidable*> getAllCollidables();
};
