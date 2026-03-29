#pragma once

#include "raylib.h"
#include "IDrawable.h"
#include "IUpdatable.h"
#include "ICollidable.h"
#include "ISpawnable.h"

/*
 * GameEntity.h
 * ------------
 * Abstract base class for all game entities (Enemy, Alien, Bullet).
 *
 * SRP: Owns only the universal entity state — position (rec), health,
 * and alive flag. Subclass-specific behaviour lives in the subclass.
 *
 * LSP: Every concrete entity (HomingEnemy, GroundAlien) IS-A GameEntity.
 * Systems that accept GameEntity* (or ICollidable*) work correctly
 * with any subclass — no special casing required.
 *
 * OCP: New entity types extend this class; the base is never modified.
 *
 * DIP: High-level systems hold ICollidable* or IDrawable*, never
 * concrete GameEntity* — they depend on the interface, not the
 * implementation.
 */

class GameEntity : public ICollidable, public IDrawable,
                   public IUpdatable, public ISpawnable {
public:
    GameEntity(float health, float width, float height);
    virtual ~GameEntity() = default;

    // ---- ICollidable ----
    Rectangle getBounds()         const override;
    bool      isAlive()           const override;
    float     getHealth()         const override;
    void      takeDamage(float amount)   override;
    void      kill()                     override;

    // ---- ISpawnable ----
    bool isActive() const override;

    // ---- IDrawable / IUpdatable are pure — subclasses must implement ----
    void draw()              const override = 0;
    void update(float dt)          override = 0;
    void spawn(float x, float y, float speed) override = 0;
    void despawn()                 override;

protected:
    Rectangle rec_;
    float     health_;
    float     maxHealth_;
    bool      alive_;
    float     velocityX_{ 0 };
    float     velocityY_{ 0 };
};
