#pragma once
#include "SessionStats.h"
#include "BulletPool.h"
#include "HomingEnemy.h"
#include "GroundAlien.h"
#include "CollisionSystem.h"
#include "EnemySpawner.h"
#include "AlienSpawner.h"
#include "PhysicsSystem.h"
#include "IObserver.h"
#include "raylib.h"
#include <vector>
#include <memory>

/*
 * GameModel — MODEL ROOT — MVC
 * ------------------------------
 * The single source of truth for all game state.
 *
 * MVC contract:
 *   MODEL   → owns all data, runs update logic, notifies observers.
 *   VIEW    → reads Model data (const refs), never writes to it.
 *   CONTROLLER → calls Model mutation methods in response to input.
 *
 * SOLID:
 *   SRP: coordinates systems — no rendering code anywhere in this file.
 *   DIP: CollisionSystem depends on ICollidable* abstractions.
 *   OCP: New entity types extend ICollidable; GameModel is unchanged.
 */
class GameModel {
public:
    static constexpr int SCREEN_W     = 1800;
    static constexpr int SCREEN_H     = 900;
    static constexpr int ENEMY_DAMAGE = 20;
    static constexpr int ALIEN_DAMAGE = 20;

    GameModel();

    void reset();
    void update(float dt);

    void addObserver   (IObserver* o) { observers_.push_back(o); }
    void removeObserver(IObserver* o);

    // ── Data accessors for View (all const) ──────────────────────────────
    const SessionStats&    getStats()         const { return stats_; }
    const BulletPool&      getBulletPool()    const { return bulletPool_; }
    const EnemySpawner&    getEnemySpawner()  const { return enemySpawner_; }
    const AlienSpawner&    getAlienSpawner()  const { return alienSpawner_; }

    Rectangle getSpaceshipPos()   const { return spaceshipPos_; }
    Rectangle getAstronautPos()   const { return astronautPos_; }
    float     getSpaceshipRot()   const { return 0.0f; }

    bool      isOnSpaceship()     const { return onSpaceship_; }
    bool      isCrashing()        const { return stats_.spaceshipDestroyed(); }
    bool      isGameOver()        const { return stats_.astronautDead(); }

    Rectangle getBgPos1()         const { return bgPos1_; }
    Rectangle getBgPos2()         const { return bgPos2_; }

    const std::vector<HitEvent>& getLastHitEvents() const { return lastHitEvents_; }

    // ── Mutation methods for Controller ──────────────────────────────────
    BulletPool&    getBulletPool()   { return bulletPool_; }
    PhysicsSystem& getPhysics()      { return physics_; }
    Rectangle&     getSpaceshipPos() { return spaceshipPos_; }
    Rectangle&     getAstronautPos() { return astronautPos_; }
    void           setOnSpaceship(bool v) { onSpaceship_ = v; }
    EnemySpawner&  getEnemySpawner() { return enemySpawner_; }

private:
    SessionStats    stats_;
    BulletPool      bulletPool_{ 50, 0.1f };
    EnemySpawner    enemySpawner_{ 10, 300.0f, 1.0f };
    AlienSpawner    alienSpawner_{ 20, 200.0f };
    CollisionSystem collisionSystem_{ stats_ };
    PhysicsSystem   physics_;

    Rectangle spaceshipPos_{ SCREEN_W * 0.2f, SCREEN_H * 0.5f, 120, 70 };
    Rectangle astronautPos_{ 300, 535, 165, 155 };

    Rectangle bgPos1_{ 0, 0, SCREEN_W, SCREEN_H };
    Rectangle bgPos2_{ SCREEN_W - 50.0f, 0, SCREEN_W, SCREEN_H };

    bool  onSpaceship_{ true };
    bool  phaseTransitioned_{ false };

    std::vector<HitEvent>  lastHitEvents_;
    std::vector<IObserver*> observers_;

    void notifyObservers();
    void updateSpaceshipPhase(float dt);
    void updateAstronautPhase(float dt);
    void scrollBackground(float dt);

    std::vector<ICollidable*> getAllCollidables();
};
